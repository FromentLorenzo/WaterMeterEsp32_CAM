from PIL import Image
import os

# Définir les chemins des dossiers
dossier_5 = './digits/6'
dossier_6 = './digits/7'
dossier_sortie = 'C:/Users/33652/Desktop/Stage/dossier_sortie'

# Assurez-vous que le dossier de sortie existe
os.makedirs(dossier_sortie, exist_ok=True)

# Obtenir la liste des fichiers dans les dossiers
images_5 = [os.path.join(dossier_5, f) for f in os.listdir(dossier_5) if f.lower().endswith(('.jpg', '.jpeg', '.png'))]
images_6 = [os.path.join(dossier_6, f) for f in os.listdir(dossier_6) if f.lower().endswith(('.jpg', '.jpeg', '.png'))]

# Afficher le nombre d'images dans chaque dossier
print(f'Nombre d\'images dans le dossier 5 : {len(images_5)}')
print(f'Nombre d\'images dans le dossier 6 : {len(images_6)}')

# Déterminer le nombre minimal d'images à traiter
nombre_images = min(len(images_5), len(images_6))

# Vérifier qu'il y a des images à traiter
if nombre_images == 0:
    print("Aucune image trouvée dans les dossiers spécifiés.")
else:
    print(f"Nombre d'images à traiter : {nombre_images}")

# Fusionner les images
for i in range(nombre_images):
    img_5_path = images_5[i]
    img_6_path = images_6[i]

    print(f'Traitement de la paire {i+1}: {img_5_path} et {img_6_path}')

    # Ouvrir les images
    img_5 = Image.open(img_5_path)
    img_6 = Image.open(img_6_path)

    # Redimensionner les images pour qu'elles aient la même largeur
    largeur_5, hauteur_5 = img_5.size
    largeur_6, hauteur_6 = img_6.size

    nouvelle_largeur = min(largeur_5, largeur_6)

    if largeur_5 != nouvelle_largeur:
        hauteur_5 = int((nouvelle_largeur / largeur_5) * hauteur_5)
        img_5 = img_5.resize((nouvelle_largeur, hauteur_5))

    if largeur_6 != nouvelle_largeur:
        hauteur_6 = int((nouvelle_largeur / largeur_6) * hauteur_6)
        img_6 = img_6.resize((nouvelle_largeur, hauteur_6))

    # Couper les images en deux
    bas_5 = img_5.crop((0, hauteur_5 // 2, nouvelle_largeur, hauteur_5))
    haut_6 = img_6.crop((0, 0, nouvelle_largeur, hauteur_6 // 2))

    # Créer une nouvelle image pour fusionner le bas de 5 avec le haut de 6
    nouvelle_image = Image.new('RGB', (nouvelle_largeur, hauteur_5))

    # Coller les morceaux dans la nouvelle image
    nouvelle_image.paste(bas_5, (0, 0))  # Coller le bas de 5 en haut
    nouvelle_image.paste(haut_6, (0, hauteur_5 // 2))  # Coller le haut de 6 en bas

    # Sauvegarder la nouvelle image
    nom_fichier = f'{i}_fusion.jpg'
    chemin_sortie = os.path.join(dossier_sortie, nom_fichier)
    nouvelle_image.save(chemin_sortie)

    print(f'Image fusionnée sauvegardée : {chemin_sortie}')
