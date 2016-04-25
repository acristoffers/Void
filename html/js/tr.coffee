###
 * Copyright (c) 2015 Álan Crístoffer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
###

window.trs = ['en', 'fr', 'de', 'pt']

tr =
    en:
        'Language': 'Language'
        'English': 'English'
        'French': 'French'
        'Portuguese': 'Portuguese'
        'German': 'German'
        'Password': 'Password'
        'Cancel': 'Cancel'
        'Open': 'Open'
        'Create': 'Create'
        'DoesntExistAndCreationIsNotPermitted': 'The selected folder is empty. To create a Store, choose Create.'
        'CantCreateCryptoObject': 'Wrong password or key.'
        'CantOpenStoreFile': 'Can\'t open the Store. Check file permissions.'
        'CantOpenFile': 'Can\'t open file. Check file permissions.'
        'CantWriteToFile': 'Can\'t write to file. Check file permissions.'
        'FileTooLarge': 'Selected file can\'t be processed: too big.'
        'NoSuchFile': 'Selected file doesn\'t exist.'
        'PartCorrupted': 'File part appears to be corrupted.'
        'WrongCheckSum': 'Wrong file checksum (i.e.: not the expected content).'
        'FileAlreadyExists': 'File already exists.'
        'Error': 'Error'
    fr:
        'Language': 'Langue'
        'English': 'Anglais'
        'French': 'Français'
        'Portuguese': 'Portugais'
        'German': 'Allemand'
        'Password': 'Mot de passe'
        'Cancel': 'Annuler'
        'Open': 'Ouvrir'
        'Create': 'Créer'
        'DoesntExistAndCreationIsNotPermitted': 'Le dossier sélectionné est vide. Pour créer un Store, choisissez Créer.'
        'CantCreateCryptoObject': 'Mot de passe ou clé faux.'
        'CantOpenStoreFile': 'Impossible d\'ouvrir le Store. Vérifiez les autorisations de fichier.'
        'CantOpenFile': 'Impossible d\'ouvrir le fichier. Vérifiez les autorisations de fichier.'
        'CantWriteToFile': 'Vous ne pouvez pas écrire dans le fichier. Vérifiez les autorisations de fichier.'
        'FileTooLarge': 'Le fichier sélectionné ne peut pas être traitée: trop gros.'
        'NoSuchFile': 'Le fichier sélectionné ne existe pas.'
        'PartCorrupted': 'Une partie du fichier semble être endommagé.'
        'WrongCheckSum': 'checksum de fichier incorrect (pas le contenu attendu).'
        'FileAlreadyExists': 'Le fichier existe déjà.'
        'Error': 'Erreur'
    pt:
        'Language': 'Idioma'
        'English': 'Inglês'
        'French': 'Francês'
        'Portuguese': 'Português'
        'German': 'Alemão'
        'Password': 'Senha'
        'Cancel': 'Cancelar'
        'Open': 'Abrir'
        'Create': 'Criar'
        'DoesntExistAndCreationIsNotPermitted': 'O diretório selecionado está vazio. Para criar uma Store, selecione criar.'
        'CantCreateCryptoObject': 'Senha ou chave incorreta.'
        'CantOpenStoreFile': 'Não foi possível abrir a Store. Verifique as permissões de arquivos.'
        'CantOpenFile': 'Não foi possível abrir um arquivo. Verifique as permissões.'
        'CantWriteToFile': 'Não foi possível escrever em um arquivo. Verifique as permissões.'
        'FileTooLarge': 'O arquivo selecionado não pôde ser processado: muito grande.'
        'NoSuchFile': 'O arquivo selecionado não existe.'
        'PartCorrupted': 'Uma parte do arquivo se corrompeu.'
        'WrongCheckSum': 'Soma de verificação errada (conteúdo não é o esperado).'
        'FileAlreadyExists': 'Arquivo já existe.'
        'Error': 'Erro'
    de:
        'Language': 'Sprache'
        'English': 'Englicsh'
        'French': 'Französisch'
        'Portuguese': 'Portuguisisch'
        'German': 'Deutsch'
        'Password': 'Passwort'
        'Cancel': 'Abbrechen'
        'Open': 'Öffnen'
        'Create': 'Erzeugen'
        'DoesntExistAndCreationIsNotPermitted': 'Der gewählte Ordner ist leer. Um ein Store zu erzeugen, klicken Sie Erzeugen.'
        'CantCreateCryptoObject': 'Passwort oder Schlüssel ist falsch.'
        'CantOpenStoreFile': 'Store kann nicht geöffnet werden. Sehen Sie die Dateiberechtigungen.'
        'CantOpenFile': 'Eine Datei kann nicht geöffnet sein. Sehen Sie die Dateiberechtigungen.'
        'CantWriteToFile': 'Eine Datei kann nicht geschrieben sein. Sehen Sie die Dateiberechtigungen.'
        'FileTooLarge': 'Die gewählte Datei kann nicht verarbeitet sein: zu groß.'
        'NoSuchFile': 'Die gewählte Datei existiert nicht.'
        'PartCorrupted': 'Dateipart scheint korrupt zu sein.'
        'WrongCheckSum': 'Falsches Checksum (Inhalt ist nicht was erwartet war).'
        'FileAlreadyExists': 'Die Datei existiert bereits.'
        'Error': 'Fehler'

window.locale = 'en'
window.tr = (str) ->
    tr[window.locale][str]

window.update_translation = ->
    attr_pattern = /data-([a-zA-Z0-9-_]+)-tr/
    $('*').each ->
        node = $(this)
        if node.attr('data-tr')?
            node.html window.tr node.attr 'data-tr'
        $.each this.attributes, ->
            if this.name.match attr_pattern
                node.attr( this.name.match(attr_pattern)[1] , window.tr node.attr(this.name) )