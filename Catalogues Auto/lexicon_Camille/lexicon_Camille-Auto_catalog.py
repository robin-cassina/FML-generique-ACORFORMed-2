'''

    Permet de faire un état des lieux des ShouldersGestures disponibles
    
    Entrée : fichier .xml d'un lexique de gestes issus de
        /BehaviorPlanner/IntentionLexicon/lexicon_Camille.xml
    Sorties : 
        
        fichier .xml de FML permettant le lancement de tous les nouveaux
        comportements sur Camille
        
        fichier .html faisant état des lieux des gestes disponibles pour ce type


'''

################# PARTIE 1 Lexicon to behaviorsets #####################

# On importe le module de regex
import re

# On ouvre le fichier en entrée
file_in = open("../../greta-master/bin/BehaviorPlanner/IntentionLexicon/lexicon_Camille.xml", "r", encoding="utf-8")

############################### EXTRACTION GESTES ######################

'''
    On crée un dictionnaire multidimensionnel *gestes* où
    --------------------------------------------------------------------
    |       clé = *classe*     |     valeur = nom du *comportement*    |
    --------------------------------------------------------------------

'''

behave=""
nbbehave = 0
liste_behave=[]
classe=""
nbcla = 0
catalog={}

# On cherche les structures de type 
#   <CLASSE category="CATEGORIE" id="NOM"> en le fichier d'entrée

regex = re.compile(r"<behaviorset name=\"([^-]*)-([\w-]*)\">")

for line in file_in :
    m = regex.search(line)
    if m :
        behave = m.group(2)
        classe = m.group(1)
        # on verifie la présence de la classe dans catalog{}
        if classe in catalog :
            # on réinitialise liste_noms[]
            liste_behave=catalog[classe]
            # on ajoute le nom à la liste
            liste_behave.append(behave)
            # on maj les clés et valeurs dans sous-gestes{}
            catalog[classe] = liste_behave
            nbbehave = nbbehave+1
        # sinon on la nouvelle classe dans catalog{}
        else :
            # on réinitialise liste_noms[]
            liste_behave=[]
            # on ajoute le nom du geste dans liste_noms[]
            liste_behave.append(behave)
            # on ajoute dans catalog{} classe => liste_behave
            catalog[classe] = liste_behave
            nbbehave, nbcla = nbbehave+1, nbcla+1

file_in.close()

################# PARTIE 1bis Lexicon Catalog ##########################

file_html = open("lexicon_Camille-List.html", "w", encoding="utf-8")

# on écrit l'en-tête, le titre, le paragraphe notant l'essentiel
file_html.write("<!DOCTYPE html>\n<html lang=\"fr\">\n"
                "<head>\n<meta charset=\"utf-8\">\n</head>\n<body>\n"
                "\t<h1>Catalogue des comportements disponibles pour Camille</h1>\n"
                "\t<h2>Behaviorsets</h2>\n"
                "\t<p>Nombre de classes disponibles : ")
file_html.write(str(nbcla))
file_html.write("<br/>Nombre de comportements disponibles : ")
file_html.write(str(nbbehave))

# on écrit aussi le tableau référençant les gestes
file_html.write("</p>\n\n\t<table border=\"1\">\n\t\t<tr>"
                "\n\t\t\t<th colspan=\"2\">Classe</th>\n"
                "\t\t</tr>\n\t\t<tr>"
                "\n\t\t\t<th>Numéro du comportement</th>"
                "\n\t\t\t<th>Nom du comportement</th>\n\t\t</tr>\n")

for classe in catalog :
    # une ligne par categorie
    file_html.write("\t\t<tr>\n")
    # avec une colonne pour la categorie
    file_html.write("\t\t\t<td colspan=\"2\"><center><b>")
    file_html.write(classe.upper())
    file_html.write("</b></center></td>\n")
    # une pour les gestes
    file_html.write("\t\t</tr>\n")
    liste_behave = catalog[classe]
    i = 1
    # print(liste_behave)
    for behave in liste_behave :
        # une rangée par comportement
        file_html.write("\t\t<tr>\n")
        # dont une colonne pour son numero
        file_html.write("\t\t\t<td>")
        file_html.write(str(i))
        file_html.write("</td>\n")
        file_html.write("\t\t\t<td>")
        file_html.write(behave)
        file_html.write("</td>\n")
        file_html.write("\t\t</tr>\n")
        i = i+1

file_html.write("\t</table>\n\n")

# fin de scipt
file_html.write("</body>\n</html>")

file_html.close()

################# PARTIE 2 Behaviorsets to FML #########################

####################### ECRITURE LANCEMENT COMPORTEMENTS ###############

for classe in catalog :    
    # On ouvre le fichier en sortie pour le script FML
    #                   (/!\ réécrit à chaque lancement)
    file_FML = open("lexicon_Camille-FMLs/FML_"+classe+".xml", "w", encoding="utf-8")

    # en-tête du script
    file_FML.write('<?xml version="1.0" encoding="utf-8" ?>\n'
        '<fml-apml>\n\t<bml>\n\t\t<speech id="s1" start="0.0"'
        ' language="english" voice="openmary" type="SAPI4" text="">\n'
        '\t\t\t<description level="1" type="gretabml">\n'
        '\t\t\t\t<reference>tmp/from-fml-apml.pho</reference>\n'
        '\t\t\t</description>\n\n')

    # on souhaite un marquage de temps par geste qui
    # retombe sur son numero
    tm=1
    liste_behave = catalog[classe]
    nbbehave = len(liste_behave)
    while tm <= nbbehave :
        file_FML.write('\t\t\t<tm id="tm')
        file_FML.write(str(tm))
        file_FML.write('"/>Le comportement que je mime est le numéro ')
        # temps de fin = tm+nbre de gestes
        file_FML.write(str(tm))
        file_FML.write('.<tm id="tm')
        file_FML.write(str(tm+nbbehave))
        file_FML.write('"/>\n')
        tm = tm+1

    file_FML.write('\t\t</speech>\n\t</bml>\n\t<fml>\n')
    tm = 1
    for behave in liste_behave :
        # on récupère notre préfixe
        file_FML.write('\t\t<')
        file_FML.write(classe)
        file_FML.write(' id="')
        file_FML.write(classe[:3])
        file_FML.write(str(tm))
        file_FML.write('" type="')
        # on récupère le nom du comportement
        file_FML.write(behave)
        file_FML.write('" start="s1:tm')
        # temps du debut = numero ordinal du geste
        file_FML.write(str(tm))
        file_FML.write('" end="s1:tm')
        # temps de fin = tm+nbre de gestes
        file_FML.write(str(tm+nbbehave))
        file_FML.write('" importance="1.0"/>\n')
        tm = tm+1

    file_FML.write('\t</fml>\n</fml-apml>')

############################### FERMETURE FICHIERS #####################

file_FML.close()
