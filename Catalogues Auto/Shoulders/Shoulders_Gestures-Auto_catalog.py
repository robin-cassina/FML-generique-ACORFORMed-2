'''

    Permet de faire un état des lieux des ShouldersGestures disponibles
    
    Entrée : fichier .xml d'un lexique de gestes issus de
        /BehaviorRealizer/BehaviorLexicon/ShouldersGestures.xml
    Sorties : 
        fichier .xml de comportements nouvellement construits qu'on 
        pourra intégrer dans le lexicon_Camille.xml
        
        fichier .xml de FML permettant le lancement de tous les nouveaux
        comportements sur Camille
        
        fichier .html faisant état des lieux des gestes disponibles pour ce type


'''

################# PARTIE 1 Lexicon to behaviorsets #####################

# On importe le module de regex
import re

# On ouvre le fichier en entrée
file_in = open("../../../../greta-master/bin/BehaviorRealizer/BehaviorLexicon/ShouldersGestures.xml", "r", encoding="utf-8")

############################### EXTRACTION GESTES ######################

'''
    On crée un dictionnaire multidimensionnel *gestes* où
    --------------------------------------------------------------------
    |       clé = *classe*     |  valeur = dictionnaire *sous_gestes*  |
    |                          |---------------------------------------|
    |                          |  clé = *categorie* |  valeur = *nom*  |
    --------------------------------------------------------------------

'''

nom=""
nbnom = 0
liste_noms=[]
categorie=""
nbcat = 0
classe=""
nbcla = 0
sous_gestes={}
gestes={}

# On cherche les structures de type 
#   <CLASSE category="CATEGORIE" id="NOM"> en le fichier d'entrée

regex = re.compile(r"<([a-zA-Z]+) id=\"([\w_]+)\" >")

for line in file_in :
    m = regex.search(line)
    if m :
        nom = m.group(2)
        categorie = m.group(1)
        classe = "shoulders"
        # on verifie la présence de la classe dans gestes{}
        if classe in gestes :
            # on verifie la presence de la categorie dans sous_gestes{}
            if categorie in sous_gestes :
                # alors on ajoute le nom à la liste
                liste_noms.append(nom)
                # et on maj la valeur de la categorie dans sous-gestes{}
                sous_gestes[categorie] = liste_noms
                nbnom = nbnom+1
            # sinon on crée un nouvelle categories dans sous-gestes{}
            else :
                # on réinitialise liste_noms[]
                liste_noms=[]
                # on ajoute le nom à la liste
                liste_noms.append(nom)
                # on maj les clés et valeurs dans sous-gestes{}
                sous_gestes[categorie] = liste_noms
                # et on maj la valeur de la classe dans gestes{}
                gestes[classe] = sous_gestes
                nbnom, nbcat = nbnom+1, nbcat+1
        # sinon on la nouvelle classe dans gestes{}
        else :
            # on réinitialise liste_noms[]
            liste_noms=[]
            # on ajoute le nom du geste dans liste_noms[]
            liste_noms.append(nom)
            # on ajoute dans sous_gestes{} categorie => liste_noms
            sous_gestes[categorie] = liste_noms
            # on ajoute dans gestes{} classe => sous_gestes
            gestes[classe] = sous_gestes
            nbnom, nbcat, nbcla = nbnom+1, nbcat+1, nbcla+1

file_in.close()

################# PARTIE 1bis Lexicon Catalog ##########################

file_html = open("Shoulders_Gestures-List.html", "w", encoding="utf-8")

# on écrit l'en-tête, le titre, le paragraphe notant l'essentiel
file_html.write("<!DOCTYPE html>\n<html lang=\"fr\">\n"
                "<head>\n<meta charset=\"utf-8\">\n</head>\n<body>\n"
                "\t<h1>Catalogue des gestes disponibles pour Camille</h1>\n"
                "\t<h2>Gestuary</h2>\n"
                "\t<p>Nombre de catégories disponibles : ")
file_html.write(str(nbcat))
file_html.write("<br/>Nombre de gestes disponibles : ")
file_html.write(str(nbnom))

# on écrit aussi le tableau référençant les gestes
file_html.write("</p>\n\n\t<table border=\"1\">\n\t\t<tr>"
                "\n\t\t\t<th colspan=\"2\">Catégorie</th>\n"
                "\t\t</tr>\n\t\t<tr>"
                "\n\t\t\t<th>Numéro du geste</th>"
                "\n\t\t\t<th>Nom du geste</th>\n\t\t</tr>\n")

for classe in gestes :
    for categorie in gestes[classe] :
        # une ligne par categorie
        file_html.write("\t\t<tr>\n")
        # avec une colonne pour la categorie
        file_html.write("\t\t\t<td colspan=\"2\"><center><b>")
        file_html.write(categorie.upper())
        file_html.write("</b></center></td>\n")
        # une pour les gestes
        file_html.write("\t\t</tr>\n")
        liste_noms = gestes[classe][categorie]
        i = 1
        for nom in liste_noms :
            # une rangée par geste
            file_html.write("\t\t<tr>\n")
            # dont une colonne pour son numero
            file_html.write("\t\t\t<td>")
            file_html.write(str(i))
            file_html.write("</td>\n")
            file_html.write("\t\t\t<td>")
            file_html.write(nom)
            file_html.write("</td>\n")
            file_html.write("\t\t</tr>\n")
            i = i+1

file_html.write("\t</table>\n\n")

# fin de scipt
file_html.write("</body>\n</html>")

file_html.close()

####################### ECRITURE NVX COMPORTEMENTS #####################
# On ouvre le fichier en sortie pour les behaviorsets
#                   (/!\ réécrit à chaque lancement)
file_out = open("Shoulders_Gestures-Planner.xml", "w", encoding="utf-8")

# en-tête du script
file_out.write('<?xml version="1.0" encoding="UTF-8"?>\n'
    '<behaviorsets xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
    'xsi:noNamespaceSchemaLocation="../../Common/Data/xml/behaviorsets.xsd">'
    '\n\n<!----------------------   CATALOGS   ----------------------->\n\n')

for classe in gestes :
    
    # on écrit un commentaire pour organiser proprement le code
    file_out.write('<!------ Shoulders ------>\n')
    
    # on crée un comportement pour chaque categorie de classe
    for categorie in gestes[classe] :
        file_out.write('<!-- ')
        file_out.write(categorie.upper())
        file_out.write(' -->\n')
        g = 1
        # on parcourt la liste des gestes
        liste_noms = gestes[classe][categorie]
        for nom in liste_noms :
            # on crée un comportement par geste disponible
            # le nom du comportement est catalogGestuary-Categorie_Nom
            file_out.write('\t<behaviorset name="catalog-')
            file_out.write(categorie)
            file_out.write(str(g))
            # on importe le signal du geste voulu
            file_out.write('">\n\t\t<signals>\n')
            file_out.write('\t\t\t<signal id="1" name="')
            file_out.write(nom)
            file_out.write('" modality="')
            file_out.write(classe)
            file_out.write('"/>\n\t\t</signals>\n\t\t<constraints>\n'
                           '\t\t\t<core>\n')
            # on ferme le chevron *signal* et on ajoute le *core*
            file_out.write('\t\t\t\t<item id="1"/>\n\t\t\t</core>\n'
                           '\t\t</constraints>\n\t</behaviorset>\n')
            g = g+1

file_out.write('<!---------------------- END CATALOGS'
                ' ----------------------->\n\n</behaviorsets>')

############################### FERMETURE FICHIERS #####################

file_out.close()

################# PARTIE 2 Behaviorsets to FML #########################

####################### ECRITURE LANCEMENT COMPORTEMENTS ###############

for classe in gestes :
    # on récupère nos timings
    tm=1
    for categorie in gestes[classe] :
        # On ouvre le fichier en sortie pour le script FML
        #                   (/!\ réécrit à chaque lancement)
        file_FML = open("Shoulders_Gestures-FMLs/FML_"+categorie+".xml", "w",
                                        encoding="utf-8")

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
        liste_noms = gestes[classe][categorie]
        nbnom = len(liste_noms)
        while tm <= nbnom :
            file_FML.write('\t\t\t<tm id="tm')
            file_FML.write(str(tm))
            file_FML.write('"/>\n')
            file_FML.write('\t\t\t\tCe geste est le numéro ')
            # temps de fin = tm+nbre de gestes
            file_FML.write(str(tm))
            file_FML.write('.\n\t\t\t<tm id="tm')
            file_FML.write(str(tm+nbnom))
            file_FML.write('"/>\n')
            file_FML.write('\t\t\t\tJ\'attends, un peu, beaucoup !\n')
            tm = tm+1

        file_FML.write('\t\t</speech>\n\t</bml>\n\t<fml>\n')
        tm = 1
        for nom in liste_noms :
            # on récupère notre préfixe
            file_FML.write('\t\t<catalog id="')
            file_FML.write(categorie[0])
            file_FML.write(str(tm))
            file_FML.write('" type="')
            # on récupère le nom du comportement
            file_FML.write(categorie)
            file_FML.write(str(tm))
            file_FML.write('" start="s1:tm')
            # temps du debut = numero ordinal du geste
            file_FML.write(str(tm))
            file_FML.write('" end="s1:tm')
            # temps de fin = tm+nbre de gestes
            file_FML.write(str(tm+nbnom))
            file_FML.write('" importance="1.0"/>\n')
            
            # et on revient à la position de repos
            file_FML.write('\t\t<catalog id="rest')
            file_FML.write(str(tm))
            file_FML.write('" type="rest14" start="s1:tm')
            # temps du debut = temps de fin du geste précédent
            file_FML.write(str(tm+nbnom))
            file_FML.write('" end="s1:tm')
            # temps de fin = début du geste suivant
            file_FML.write(str(tm+1))
            file_FML.write('" importance="1.0"/>\n')
            tm = tm+1

        file_FML.write('\t</fml>\n</fml-apml>')

############################### FERMETURE FICHIERS #####################

file_FML.close()
