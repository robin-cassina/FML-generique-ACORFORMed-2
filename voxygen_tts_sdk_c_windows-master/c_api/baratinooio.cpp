/* $Id: baratinooio.cpp 28099 2017-01-30 08:53:34Z paul $ -*- coding: utf-8-unix -*- */

/* Implementation of the InputTextBuffer and OutputSignalBuffer objects */

#include <string.h>
#include "baratinooio.h"

#define WAV_INFO_CHUNK

#if defined WAV_INFO_CHUNK
#include <stdio.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif
#endif

//---------------------------------------------------------------------------------
//                          Méthodes InputTextBuffer 
//---------------------------------------------------------------------------------
InputTextBuffer *InputTextBuffer::newInstance(BARATINOO_PARSING parsing, BARATINOO_TEXT_ENCODING encoding, int voiceIndex, char *voiceModules) {
  return new InputTextBuffer(parsing, encoding, voiceIndex, voiceModules);
}
void InputTextBuffer::deleteInstance(InputTextBuffer *instance) {
  delete instance;
}

//---------------------------------------------------------------------------------
InputTextBuffer::InputTextBuffer(BARATINOO_PARSING parsing, BARATINOO_TEXT_ENCODING encoding, int voiceIndex, char *voiceModules):InputText(parsing, encoding, voiceIndex, voiceModules),
    _buffer(NULL),
    _p(NULL),
    _length(0)
{}

//---------------------------------------------------------------------------------
int InputTextBuffer::init(const char *text)
{
    if (text == NULL)
        return 0;

    // on garde une copie du texte. Si _buffer est déjà utilisé, il est libéré avant
    if (_buffer != NULL)
    {
        delete[] _buffer;
        _buffer = NULL;
    }
    _length = strlen(text);
    _buffer = new char[_length+1];
    if (_buffer == NULL)
        return 0;
    strncpy(_buffer,text,_length+1);
    _buffer[_length] = '\0';
    _p = _buffer;       // init current pointer

    return 1;    
}

//---------------------------------------------------------------------------------
InputTextBuffer::~InputTextBuffer()
{
    if (_buffer != NULL)
        delete[] _buffer;
}

//---------------------------------------------------------------------------------
int InputTextBuffer::readText(void *address, int length)
{
    if (_length == 0 || length <= 0)       // plus rien à renvoyer
        return 0;
    else if ((size_t) length > _length)
	length = (int) _length;
    
    memcpy(address, _p, length);
    _p += length;
    _length -= length;
    
    return length;
}


//---------------------------------------------------------------------------------
//                          Méthodes OutputSignalBuffer 
//---------------------------------------------------------------------------------


/* Macros de swap */
#define SWAP_INT16(x) ((((unsigned short)(x))>>8)|(((unsigned short)(x))<<8))
#define HI_INT32(x) (((unsigned int)(x))>>16)
#define LOW_INT32(x) ((unsigned int)(x))
#define SWAP_INT32(x) (((unsigned int)SWAP_INT16(HI_INT32(x)) & 0x0000FFFF) | (((unsigned int)SWAP_INT16(LOW_INT32(x)))<<16))


OutputSignalBuffer *OutputSignalBuffer::newInstance(BARATINOO_SIGNAL_CODING coding, int frequency) {
  return new OutputSignalBuffer(coding, frequency);
}
void OutputSignalBuffer::deleteInstance(OutputSignalBuffer *instance) {
  delete instance;
}

//---------------------------------------------------------------------------------
OutputSignalBuffer::OutputSignalBuffer(BARATINOO_SIGNAL_CODING coding, int frequency):OutputSignal(coding, frequency),
    _buffer(NULL),
    _length(0),
    _error(0)
{}

//---------------------------------------------------------------------------------
OutputSignalBuffer::~OutputSignalBuffer()
{
    resetSignal();
}

//---------------------------------------------------------------------------------
int OutputSignalBuffer::writeSignal(const void *address, int length)
{
    if ((address == NULL) || (length < 0))      // pas de signal, on ne fait rien
        return 0;
    
    // on réalloue la taille nécessaire
    char *newBuffer = new char[length+_length];
    if (newBuffer == NULL)
    {   // le signal est perdu
        _error = 1;
        return 0;
    }    
        
    // s'il existe déjà du signal, on le recopie au début du nouveau buffer
    if (_length > 0)
    {
        memcpy(newBuffer, _buffer, _length);
        delete[] _buffer;
    }
    memcpy(newBuffer+_length, address, length);
    _buffer = newBuffer;
    _length += length;
    
    return 0;
}

//---------------------------------------------------------------------------------
void OutputSignalBuffer::resetSignal()
{
    if (_buffer != NULL)
    {
        delete[] _buffer;
		_buffer = NULL;
        _length = 0;
    }
    _error = 0;
}



//---------------------------------------------------------------------------------
void* OutputSignalBuffer::detachSignal()
{
    if (_buffer != NULL)
    {
         void* pOldBuffer = (void*)_buffer;
		_buffer = NULL;     // pas de delete, on met juste le pointeur à null
        _length = 0;
        return pOldBuffer;
    }
    return NULL;
}



//---------------------------------------------------------------------------------
// Renvoie la longueur de l'entête et le pointeur sur l'entête dans pHeader ou
// une valeur négative ou nulle en cas d'erreur :
//  0 format d'entête inconnu
// -1 erreur allocation mémoire
// -2 codage de signal inconnu
// Le pointeur retourné est alloué dynamiquement et doit être libéré par un appel à la fonction deleteSignalHeader
int OutputSignalBuffer::getSignalHeader(HEADER_FORMAT format, char **pHeader)
{
    int ret;
    *pHeader = NULL;
    
    if (format == WAV_HEADER)
        ret = codeWavHeader(pHeader);
    else if (format == AU_HEADER)
        ret = codeAuHeader(pHeader);
    else ret = 0;   // format d'entête inconnu
    return ret;        
}


void OutputSignalBuffer::deleteSignalHeader(char *header) {
  if (header != NULL)
    delete[] header;
}

//---------------------------------------------------------------------------------
// Code un entête au format WAV
// Retourne la longueur de l'entête et un pointeur sur l'entête dans pHeader ou, si erreur :
// -1 erreur allocation mémoire
// -2 codage de signal inconnu
//
// the file "FormatWAV_fmt.html" included in this directory deals with each of these parametres
int OutputSignalBuffer::codeWavHeader( char **pHeader)
{
    unsigned short  audioFormat;
    unsigned short  numChannel;
    unsigned short  bitPerSample;
    unsigned short  blockAlign;
    unsigned int    sampleRate;
    unsigned int    byteRate;
    unsigned int    chunkSize;
    unsigned int    fmtChunkSize;
    unsigned int    dataChunkSize;
    int             headerSize;
    int             swap;
    short           byteOrder = 1;

#ifdef WAV_INFO_CHUNK
#define ALIGN_32(x) (((x)&3)?(((x)+3)&~3):(x))
#define CRD_SIZE 11		// "yyyy-mm-dd" ISO-8601 date
    unsigned int    isftChunkSize;
    unsigned int    icrdChunkSize;
    unsigned int    listChunkSize;
    const char     *version = getBaratinooVersion();
    const char      software[11] = "Baratinoo:";
    char	    creationDate[CRD_SIZE] = "1970-01-01";

#if defined _WIN32
    SYSTEMTIME stime;
    GetSystemTime(&stime);
    _snprintf (creationDate, CRD_SIZE, "%04d-%02d-%02d", stime.wYear, stime.wMonth, stime.wDay);
#else
    time_t t;
    struct tm tm;

    time(&t);
    if(gmtime_r(&t, &tm) != NULL)
      snprintf (creationDate, CRD_SIZE, "%04d-%02d-%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
#endif

    isftChunkSize = (unsigned int)(strlen(software) + strlen(version) + 1);
    isftChunkSize = ALIGN_32(isftChunkSize);
    icrdChunkSize = ALIGN_32(CRD_SIZE);
    listChunkSize = 4 /* INFO */ + 8 /* ISFT chunk header */ + isftChunkSize + 8 /* ICRD chunk header */ + icrdChunkSize;
#endif

    *pHeader = 0;
    switch (_coding)
    {
        case BARATINOO_A_LAW:
        case BARATINOO_U_LAW:
            fmtChunkSize = 18;
            headerSize = 46;
            break;
        case BARATINOO_PCM:
        case BARATINOO_PCM_SWAP:
        case BARATINOO_PCM_LE:
        case BARATINOO_PCM_BE:
        case BARATINOO_VOX:
            fmtChunkSize = 16;
            headerSize = 44;
            break;
        default:
            return -2;
    }
#ifdef WAV_INFO_CHUNK
    headerSize += 8 /* LIST chunk header */ + listChunkSize;
#endif
    dataChunkSize = _length;
    chunkSize = headerSize - 8 /* RIFF chunk header */ + dataChunkSize;

    
    // Ecriture du premier chunk 'RIFF'
    //---------------------------------
    char *header = new char[headerSize];
    *pHeader = header;
    if (header == NULL)
        return -1;
    
    // ordre des octets
    if (   (_coding == BARATINOO_PCM_BE)
        || ((_coding == BARATINOO_PCM_SWAP) && *((char*)&byteOrder)) )   // swap demandé et plate-forme Intel => byteorder MOTOROLA pour le signal
        memcpy( header, "RIFX", 4);     // ordre a la Motorola
    else memcpy( header, "RIFF", 4);    // ordre a la Intel

    if (   ((*(header+3) == 'X') && *((char*)&byteOrder))            // fichier Motorola et plate-forme Intel
        || ((*(header+3) == 'F') && !*((char*)&byteOrder)) )         // fichier Intel et plate-forme Motorola
        swap = 1;       // swap des champs nécessaires pour respecter l'ordre des octets
    else swap = 0;
    header += 4;

    if (swap)
        chunkSize = SWAP_INT32(chunkSize);
    memcpy(header, (const void*)&chunkSize, 4); header+=4;
    memcpy(header, "WAVE", 4); header+=4;       // fichier RIFF au format WAVE

    // Ecriture du chunk 'fmt ' (format du Wave), il est toujours avant le chunk 'data'
    //-------------------------

    // on prépare les différents champs du chunk avant de les écrire
    switch (_coding)
    {
        case BARATINOO_A_LAW:
            audioFormat = 0x0006; // WAVE_FORMAT_ALAW
            bitPerSample = 8;
            break;
        case BARATINOO_U_LAW:
            audioFormat = 0x0007; // WAVE_FORMAT_MULAW
            bitPerSample = 8;
            break;
        case BARATINOO_PCM:
        case BARATINOO_PCM_SWAP:
        case BARATINOO_PCM_LE:
        case BARATINOO_PCM_BE:
            audioFormat = 0x0001; // WAVE_FORMAT_PCM
            bitPerSample = 16;
            break;
        case BARATINOO_VOX:
            audioFormat = 0x0062; // WAVE_FORMAT_VOXWARE (used as private encoding)
            bitPerSample = 16;
            break;
        default:
            return -2;
    }
    numChannel = 1;
    sampleRate = _frequency;
    byteRate = sampleRate * numChannel * (bitPerSample/8);
    blockAlign = numChannel * (bitPerSample/8);
    
    if (swap)
    {
        audioFormat = SWAP_INT16(audioFormat);
        numChannel = SWAP_INT16(numChannel);
        bitPerSample = SWAP_INT16(bitPerSample);
        blockAlign = SWAP_INT16(blockAlign);
        
        fmtChunkSize = SWAP_INT32(fmtChunkSize);
        sampleRate = SWAP_INT32(sampleRate);
        byteRate = SWAP_INT32(byteRate);
    }
    // écriture
    memcpy(header, "fmt ", 4); header+=4;
    memcpy(header, (const void*)&fmtChunkSize, 4); header+=4;
    memcpy(header, (const void*)&audioFormat, 2); header+=2;
    memcpy(header, (const void*)&numChannel, 2); header+=2;
    memcpy(header, (const void*)&sampleRate, 4); header+=4;
    memcpy(header, (const void*)&byteRate, 4); header+=4;
    memcpy(header, (const void*)&blockAlign, 2); header+=2;
    memcpy(header, (const void*)&bitPerSample, 2); header+=2;
    if((_coding == BARATINOO_A_LAW) || (_coding == BARATINOO_U_LAW)) {
        short cbsize = 0;
        memcpy(header, (const void*)&cbsize, 2); header+=2;
    }
   
#ifdef WAV_INFO_CHUNK
    // Ecriture du chunk 'LIST'
    //-------------------------

    size_t slen, pad;
    if (swap)
        listChunkSize = SWAP_INT32(listChunkSize);
    memcpy(header, "LIST", 4); header+=4;
    memcpy(header, (const void*)&listChunkSize, 4); header+=4;
    memcpy(header, "INFO", 4); header+=4;
    memcpy(header, "ISFT", 4); header+=4;	// Software
    pad = isftChunkSize;
    if (swap)
        isftChunkSize = SWAP_INT32(isftChunkSize);
    memcpy(header, (const void*)&isftChunkSize, 4); header+=4;
    slen = strlen(software);
    memcpy(header, software, slen); header+= slen;
    pad -= slen;
    slen = strlen(version) + 1;
    memcpy(header, version, slen); header+= slen;
    pad -= slen;
    memset(header, 0, pad); header += pad;
    memcpy(header, "ICRD", 4); header+=4;	// Creation Date
    pad = icrdChunkSize;
    if (swap)
        icrdChunkSize = SWAP_INT32(icrdChunkSize);
    memcpy(header, (const void*)&icrdChunkSize, 4); header+=4;
    memcpy(header, creationDate, CRD_SIZE); header+=CRD_SIZE;
    pad -= CRD_SIZE;
    memset(header, 0, pad); header += pad;
#endif

    // Ecriture du chunk 'data'
    //-------------------------
    if (swap)
        dataChunkSize = SWAP_INT32(dataChunkSize);
    memcpy(header, "data", 4); header+=4;
    memcpy(header, (const void*)&dataChunkSize, 4); header+=4;
    // on ne s'occupe pas des données        
        
    return headerSize;
}




//---------------------------------------------------------------------------------
// Code un entête au format AU
// Retourne la longueur de l'entête et un pointeur sur l'entête dans pHeader ou, si erreur :
// -1 erreur allocation mémoire
// -2 codage de signal inconnu
int OutputSignalBuffer::codeAuHeader( char **pHeader)
{
	unsigned int	hdr_size;	    /* size of this header (optional) */
	unsigned int	data_size;	    /* length of data (optional) */
	unsigned int	encoding;	    /* data encoding format */
	unsigned int	sample_rate;	/* samples per second */
	unsigned int	channels;	    /* number of interleaved channels */
    int             swap;
    short           byteOrder = 1;

    char *header = new char[24];
    if (header == NULL)
        return -1;
    
    // ordre des octets
    if (   (_coding == BARATINOO_PCM_BE)
        || ((_coding == BARATINOO_PCM_SWAP) && *((char*)&byteOrder)) )   // swap demandé et plate-forme Intel => byteorder MOTOROLA pour le signal
        memcpy( header, ".snd", 4);     // ordre a la Motorola
    else memcpy( header, "dns.", 4);    // ordre a la Intel

    if (   ((*header == '.') && *((char*)&byteOrder))            // fichier Motorola et plate-forme Intel
        || ((*header == 'd') && !*((char*)&byteOrder)) )         // fichier Intel et plate-forme Motorola
        swap = 1;       // swap des champs nécessaires pour respecter l'ordre des octets
    else swap = 0;

    hdr_size = 24;
    data_size = _length;
    switch (_coding)
    {
        case BARATINOO_A_LAW:
            encoding = 27;
            break;
        case BARATINOO_U_LAW:
            encoding = 1;
            break;
        case BARATINOO_PCM:
        case BARATINOO_PCM_SWAP:
        case BARATINOO_PCM_LE:
        case BARATINOO_PCM_BE:
            encoding = 3;
            break;
        case BARATINOO_VOX:
            encoding = 8; // Fragmented sample data (used as private encoding)
            break;
        default:
            delete []header;
            return -2;
    }
    sample_rate = _frequency;
    channels = 1;
    
    if (swap)
    {   
        hdr_size    = SWAP_INT32(hdr_size);
        data_size   = SWAP_INT32(data_size);
        encoding    = SWAP_INT32(encoding);
        sample_rate = SWAP_INT32(sample_rate);
        channels    = SWAP_INT32(channels);
    }
    memcpy(header+4, (const void*)&hdr_size, 4);
    memcpy(header+8, (const void*)&data_size, 4);
    memcpy(header+12, (const void*)&encoding, 4);
    memcpy(header+16, (const void*)&sample_rate, 4);
    memcpy(header+20, (const void*)&channels, 4);
    
    *pHeader = header;
    return 24;
}
