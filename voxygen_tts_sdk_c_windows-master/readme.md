# Voxygen TTS SDK

Voxygen is a C/C++ library of TTS.

For instalation see [VOX167_SDK_C_8.1_1.0_EN.pdf](VOX167_SDK_C_8.1_1.0_EN.pdf), part 3.

For ACORFORMed, in September 2017, Voxygen replace the CereProc TTS in the GRETA platform. 
We wrap the C++ library in Java using [SWIG](http://www.swig.org/).
<br>Details are in the [c_api/baratinSwig/README.md](c_api/baratinSwig/README.md)

Two little branches contain the different licence files:
- `lic-laptopGreg-win64` : Gregoire laptop &rArr; `config/V8.1_1748.lic`
- `lic-laptopDemo-win64` : Demo laptop &rArr; `config/V8.1_1872.lic`

Nota: In June 2017 we changed of version (8.0 &rarr; 8.1) to get the *viseme* output.
