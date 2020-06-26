# FML-generique-ACORFORMed-2

## Installation de Greta
Vous pouvez récupérer ma version qui sera directement fonctionnelle (une fois la [licence voxygen](#licence-voxygen) ajoutée). Mes modifications sur le FMLAnnotator y seront directement intégrées.

Vous pouvez également repartir de la version actuelle de Greta (fonctionne au 26 juin 2020), pour cela il vous faudra :

Télécharger ou cloner greta depuis Github : https://github.com/isir/greta

Copier dans <greta-master>\ le contenu de « Fichiers annexes pour Greta», remplacer les fichiers.
Ces éléments sont ceux nécessaires au bon fonctionnement de Voxygen : 
- <Greta_DIR>\Common\Data\Voxygen\audio\ 
- <Greta_DIR>\Common\Data\Voxygen\data\ 
- <Greta_DIR>\Common\Data\Voxygen\baratinoo.cfg 
- <Greta_DIR>\Common\Lib\External\voxygen\Win64\baratinSwig.dll 
- <Greta_DIR>\Common\Lib\External\voxygen\Win64\libbaratinoo.dll

Compiler à l'aide de NetBeans ou avec Apache ANT 

J'ai personnellement utilisé ANT.
Explications trouvées sur le [Wiki](https://github.com/isir/greta/wiki) de Greta puis traduites de l'anglais:
## Build via script
### ANT Installation and Setup (in Windows)
Vous pouvez téléchargez ANT depuis https://ant.apache.org/. J'ai personnellement utilisé la version 1.10.6 ([Lien direct](https://archive.apache.org/dist/ant/binaries/apache-ant-1.10.6-bin.zip)). 

Télécharger la distribution binaire, décompresser la, et mettez la à l'emplacement de votre choix. Après cela, créer ces variables d'environnement, toutes trois dans la section 'Variables utilisateur' (si vous ne savez pas le faire, voici une vidéo d'exemple : https://www.youtube.com/watch?v=83SccoBYSfA ).

Voici comment changer les variables d’environnement :

Ouvrir le panneau de configuration (touche Windows +R puis tapez “control” et cliquer sur Ok)

Une fois dans le panneau de configuration, allez dans “Système et sécurité”, puis “Système”, puis sur la gauche “Paramètres systèmes avancés”, tout en bas de la fenêtre qui vient de s’ouvrir cliquez sur le bouton “Variables d’environnement”.

Dans la nouvelle fenêtre créer les nouvelles variables utilisateur suivantes :
1.	ANT_HOME: <emplacement-du-dossier-ANT-dézippé>
2.	Path: ajouter <emplacement-du-dossier-ANT-dézippé\bin> 

(Cliquer sur Path, puis “Modifier..”, et dans la nouvelle fenêtre “Nouveau”
3.	JAVA_HOME: <emplacemet-de-votre-JDK-par-défaut> (vous pourriez l'avoir déjà ajoutée)

Vous êtes prêt à utiliser ANT! (n'oubliez pas de fermer les invites de commande windows en cours d'execution au passage)
#### Exécuter les scripts ANT Build
Pour build depuis un état propre, ouvrez l'invite de commande, rendez-vous à l'emplacement de votre dossier <GRETA_DIR>/ et exécutez la commande : 
```
ant build
```

Note : Vous pouvez modifier quels fichiers vont être compilés en modifiant le fichier build.xml qui se trouve à la racine de greta-master.
#### Exécuter les scripts ANT Clean 
Pour clean depuis un état compilé, ouvrez l'invite de commande, rendez-vous à l'emplacement de votre dossier <GRETA_DIR>/ et exécutez la commande :  
```
ant clean
```


 
## Licence Voxygen
Le projet utilise le TTS Voxygen.
Il faut exécuter lic_id.exe sur votre machine (voxygen_tts_sdk_c_windows-master\bin\<plateforme>). Cela va vous générer le fichier baratinoo.uid qu'il faut ensuite envoyer à support@voxygen.fr pour qu'ils puissent vous renvoyer la licence destinée à votre PC.  Référence du projet Acorformed : (ref 16D348)
 
Une fois le fichier de licence reçu, il faut le placer dans le même répertoire que le fichier baratinoo.cfg (<Greta_DIR>\Common\Data\Voxygen)

## Premier lancement
Le dossier bin contient l’application exécutable et entièrement fonctionnel, je le nommerai <Greta_DIR> dorénavant.
Lancer <Greta_DIR>\Modular.jar
Puis fichier -> Ouvrir -> choisir <Greta_DIR>\Configurations\GretaUnity\Projects\ACORFORMed\ACORFORMed – Voxygen.xml

Vous pouvez aussi choisir une autre configuration ( « Basic configuration » par exemple) et remplacer le TTS (CereProc ou MaryTTS) par Voxygen, pour cela il faut d’abord ajouter un accès au module Voxygen.

Dans le Modular, choississez l’onglet Edition → Menu Naviguez jusqu’à « TTS » puis Nouvel Item, Nom : Voxygen et choisir le module Voxygen.

Enregistrer.

Puis dans le menu, Fichier -> Quitter pour fermer Greta.

Relancer le.

Maintenant pour choisir Voxygen plutôt que celui présent dans votre configuration actuelle :
- Cliquer sur le character manager (cadre rouge/rose)
- Dans le menu, Ajouter -> TTS -> Voxygen (c’est l’item que vous venez d’ajoutez si vous avez bien réaliser les précédentes manipulationss)
- Relier TTS au nouveau module (Tirer une flèche depuis TTS vers Voxygen)


Pour ajouter le module de Text to FML (FML Annotator)

Dans le Modular, cliquer sur le Character Manager, puis Ajouter -> Inputs -> FML Annotator

Relier celui-ci au Behavior Planner

Pour l'utiliser, cliquer dessus puis il va s'ouvrir dans le bas de votre fenêtre Modular


## FML Annotator

J'ai effectué quelques modifications du FML annotator. 

Pour que ces modifications soient effectives sur une nouvelle intallation de Greta, il faut récupérer les fichiers Model.java et View.java qui sont présents sur GitHub [FML-generique-ACORFORMed-2/greta-master/auxiliary/FMLAnnotator/src/greta/auxiliary/fmlannotator/](https://github.com/robin-cassina/FML-generique-ACORFORMed-2/tree/master/greta-master/auxiliary/FMLAnnotator/src/greta/auxiliary/fmlannotator) et les replacer au même emplacement de votre configuration de Greta, avant de recompiler.



## Dictionnaires/DialogActs
Les dictionnaires utilisés par le FMLAnnotator se trouvent dans <Greta_DIR>\Common\Data\FMLAnnotator\Configs\

Les informations concernant les balises \<social>, \<turntaking>, \<backchannel>, \<performative>, \<functional-relation>, \<emotion>, \<emphasis>, \<certainty> et \<other> se trouvent dans la [documentation](https://github.com/robin-cassina/FML-generique-ACORFORMed-2/blob/master/Docs%20FML/fml-aamas.pdf) du langage FML-APML.

## Documentations FML-APML et SSML
Les informations concernant la porsodie se trouve dans la [documentation](https://www.w3.org/TR/speech-synthesis11/#S3.2) du langage SSML sur le site W3C (ou dans le dépot git à l'emplacement suivant : [FML-generique-ACORFORMed-2/Docs FML/Speech Synthesis Markup Language (SSML) Version 1.1 W3C.html](https://github.com/robin-cassina/FML-generique-ACORFORMed-2/blob/master/Docs%20FML/Speech%20Synthesis%20Markup%20Language%20(SSML)%20Version%201.1%20W3C.html))
  


## Emotions qui fonctionnent avec Greta/Voxygen
<table>
    <tr>
        <td>1</td>
        <td>panic_fear</td>
        <td>bug</td>
    </tr>    
    <tr>
        <td>2</td>
        <td>anxiety</td>
        <td>non</td>
    </tr>
    <tr>
        <td>3</td>
        <td>sadness</td>
        <td>oui?</td>
    </tr>
    <tr>
        <td>4</td>
        <td>envy</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>5</td>
        <td>anger</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>6</td>
        <td>joyStrong</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>7</td>
        <td>joy</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>8</td>
        <td>despair</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>9</td>
        <td>disgust</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>10</td>
        <td>hate</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>11</td>
        <td>worried</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>12</td>
        <td>disappointment</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>13</td>
        <td>neutral</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>14</td>
        <td>pride</td>
        <td>bug</td>
    </tr>
    <tr>
        <td>15</td>
        <td>tension</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>16</td>
        <td>relief</td>
        <td>bug</td>
    </tr>
    <tr>
        <td>17</td>
        <td>embarrassment</td>
        <td>bug</td>
    </tr>
    <tr>
        <td>18</td>
        <td>surprise</td>
        <td>non</td>
    </tr>
    <tr>
        <td>19</td>
        <td>bored</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>20</td>
        <td>small_sadness</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>21</td>
        <td>small_anger</td>
        <td>oui</td>
    </tr>
    <tr>
        <td>22</td>
        <td>small_joy</td>
        <td>oui</td>
    </tr>
</table>
oui : Fonctionne.

non : Ne fontionne pas.

bug : Fais crash Voxygen.

## Catalogues Auto
Des informations sur les catalogues automatiques sont présentes dans le rapport de stage (html) de Victoria ROSA qui en est l'auteur et qui se trouve sur le Git dans le dossier [FML-generique-ACORFORMed-2/Catalogues Auto/rapport-stage_PEPS-2018_Victoria_ROSA.html](https://github.com/robin-cassina/FML-generique-ACORFORMed-2/blob/master/Catalogues%20Auto/rapport-stage_PEPS-2018_Victoria_ROSA.html)

