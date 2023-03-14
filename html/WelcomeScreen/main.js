(window["webpackJsonp"] = window["webpackJsonp"] || []).push([["main"],{

/***/ 0:
/*!***************************!*\
  !*** multi ./src/main.ts ***!
  \***************************/
/*! no static exports found */
/***/ (function(module, exports, __webpack_require__) {

module.exports = __webpack_require__(/*! /home/alan/Developer/void-cpp/WelcomeScreen/src/main.ts */"zUnb");


/***/ }),

/***/ "0hYy":
/*!***********************************!*\
  !*** ./src/app/translation/pt.ts ***!
  \***********************************/
/*! exports provided: LANG_PT_NAME, LANG_PT_TRANS */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "LANG_PT_NAME", function() { return LANG_PT_NAME; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "LANG_PT_TRANS", function() { return LANG_PT_TRANS; });
/*
Copyright (c) 2016 Álan Crístoffer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
// tslint:disable:max-line-length
var LANG_PT_NAME = 'pt';
var LANG_PT_TRANS = {
    'English': 'Inglês',
    'French': 'Francês',
    'German': 'Alemão',
    'Portuguese': 'Português',
    'Language': 'Idioma',
    'Password': 'Senha',
    'Create': 'Criar',
    'Open': 'Abrir',
    'Cancel': 'Cancelar',
    'The selected folder is empty. To create a Store, choose Create.': 'O diretório selecionado está vazio. Para criar uma Store, clique em criar.',
    'Wrong password or key.': 'Senha ou chave erradas.',
    'Could not open the Store. Check file permissions.': 'Não foi possível abrir a Store. Verifique as permissões de arquivos.',
    'Could not open file. Check file permissions.': 'Não foi possível abrir o arquivo. Verifique as permissões de arquivos.',
    'Could not write to file. Check file permissions.': 'Não foi possível escrever no arquivo. Verifique as permissões de arquivos.',
    'Selected file Could not be processed: too big.': 'O arquivo selecionado não pode ser processado: muito grande.',
    'Selected file does not exist.': 'O arquivo selecionado não existe',
    'File part appears to be corrupted.': 'Parte do arquivo parece estar corrompido.',
    'Wrong file checksum (i.e.: not the expected content).': 'Checksum errado (conteúdo inesperado).',
    'File already exists.': 'O arquivo já existe.',
    'Password should have at least 4 characters.': 'A senha deve ter no mínimo 4 caractéres.'
};


/***/ }),

/***/ "9fjc":
/*!****************************************************!*\
  !*** ./src/app/translation/translation.service.ts ***!
  \****************************************************/
/*! exports provided: TranslateService */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TranslateService", function() { return TranslateService; });
/* harmony import */ var tslib__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! tslib */ "mrSG");
/* harmony import */ var _angular_core__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @angular/core */ "fXoL");
/* harmony import */ var _translation__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./translation */ "jroB");
/*
Copyright (c) 2016 Álan Crístoffer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/



var TranslateService = /** @class */ (function () {
    function TranslateService(_translations) {
        this._translations = _translations;
    }
    Object.defineProperty(TranslateService.prototype, "currentLang", {
        get: function () {
            return this._currentLang;
        },
        enumerable: false,
        configurable: true
    });
    TranslateService.prototype.use = function (lang) {
        this._currentLang = lang;
        localStorage.setItem('language', lang);
    };
    TranslateService.prototype.translate = function (key) {
        var lang = this._translations.get(this.currentLang) || {};
        return lang[key] || key;
    };
    TranslateService.prototype.instant = function (key) {
        return this.translate(key);
    };
    TranslateService.ctorParameters = function () { return [
        { type: undefined, decorators: [{ type: _angular_core__WEBPACK_IMPORTED_MODULE_1__["Inject"], args: [_translation__WEBPACK_IMPORTED_MODULE_2__["TRANSLATIONS"],] }] }
    ]; };
    TranslateService = Object(tslib__WEBPACK_IMPORTED_MODULE_0__["__decorate"])([
        Object(_angular_core__WEBPACK_IMPORTED_MODULE_1__["Injectable"])(),
        Object(tslib__WEBPACK_IMPORTED_MODULE_0__["__metadata"])("design:paramtypes", [Object])
    ], TranslateService);
    return TranslateService;
}());



/***/ }),

/***/ "AytR":
/*!*****************************************!*\
  !*** ./src/environments/environment.ts ***!
  \*****************************************/
/*! exports provided: environment */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "environment", function() { return environment; });
// This file can be replaced during build by using the `fileReplacements` array.
// `ng build --prod` replaces `environment.ts` with `environment.prod.ts`.
// The list of file replacements can be found in `angular.json`.
var environment = {
    production: false
};
/*
 * For easier debugging in development mode, you can import the following file
 * to ignore zone related error stack frames such as `zone.run`, `zoneDelegate.invokeTask`.
 *
 * This import should be commented out in production mode because it will have a negative impact
 * on performance if an error is thrown.
 */
// import 'zone.js/dist/zone-error';  // Included with Angular CLI.


/***/ }),

/***/ "Sy1n":
/*!**********************************!*\
  !*** ./src/app/app.component.ts ***!
  \**********************************/
/*! exports provided: AppComponent */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "AppComponent", function() { return AppComponent; });
/* harmony import */ var tslib__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! tslib */ "mrSG");
/* harmony import */ var _raw_loader_app_component_html__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! raw-loader!./app.component.html */ "VzVu");
/* harmony import */ var _app_component_scss__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./app.component.scss */ "ynWL");
/* harmony import */ var _angular_core__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! @angular/core */ "fXoL");
/* harmony import */ var _translation_translation_service__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! ./translation/translation.service */ "9fjc");
/* harmony import */ var _angular_material_snack_bar__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! @angular/material/snack-bar */ "dNgK");
/* harmony import */ var angular2_hotkeys__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! angular2-hotkeys */ "yl57");







var AppComponent = /** @class */ (function () {
    function AppComponent(translate, toast, zone, hotkeys) {
        var _this = this;
        this.translate = translate;
        this.toast = toast;
        this.zone = zone;
        this.hotkeys = hotkeys;
        this.isNew = false;
        this.storePath = false;
        this.password = '';
        this.hotkeys.add(new angular2_hotkeys__WEBPACK_IMPORTED_MODULE_6__["Hotkey"](['meta+o', 'ctrl+o'], function (_) {
            _this.select();
            return false;
        }));
        this.hotkeys.add(new angular2_hotkeys__WEBPACK_IMPORTED_MODULE_6__["Hotkey"](['meta+n', 'ctrl+n'], function (_) {
            _this.create();
            return false;
        }));
    }
    AppComponent.prototype.ngOnInit = function () {
        var _this = this;
        bridge.lang(function (lang) {
            _this.zone.run(function () {
                _this.translate.use(lang || 'en');
            });
        });
    };
    AppComponent.prototype.setLanguage = function (lang) {
        bridge.setLang(lang);
        this.translate.use(lang);
    };
    AppComponent.prototype.create = function () {
        var _this = this;
        bridge.getNew(function (path) {
            _this.zone.run(function () {
                _this.storePath = path;
                _this.isNew = true;
            });
        });
    };
    AppComponent.prototype.open = function () {
        var _this = this;
        if (this.password.trim().length < 4) {
            var msg = this.translate.instant('Password should have at least 4 characters.');
            this.toast.open(msg, null, { duration: 2000 });
            return;
        }
        var action = this.isNew ? bridge.create : bridge.open;
        action(this.password, function (result) {
            _this.zone.run(function () {
                if (result === 'Success') {
                    bridge.close();
                }
                else {
                    var messages = {
                        'DoesntExistAndCreationIsNotPermitted': 'The selected folder is empty. To create a Store, choose Create.',
                        'CantCreateCryptoObject': 'Wrong password or key.',
                        'CantOpenStoreFile': 'Could not open the Store. Check file permissions.',
                        'CantOpenFile': 'Could not open file. Check file permissions.',
                        'CantWriteToFile': 'Could not write to file. Check file permissions.',
                        'FileTooLarge': 'Selected file Could not be processed: too big.',
                        'NoSuchFile': 'Selected file does not exist.',
                        'PartCorrupted': 'File part appears to be corrupted.',
                        'WrongCheckSum': 'Wrong file checksum (i.e.: not the expected content).',
                        'FileAlreadyExists': 'File already exists.'
                    };
                    var msg = _this.translate.instant(messages[result]);
                    _this.toast.open(msg, null, { duration: 2000 });
                }
            });
        });
    };
    AppComponent.prototype.select = function () {
        var _this = this;
        bridge.getExisting(function (path) {
            _this.zone.run(function () {
                _this.storePath = path;
                _this.isNew = false;
            });
        });
    };
    AppComponent.prototype.cancel = function () {
        this.storePath = false;
        this.password = '';
    };
    AppComponent.ctorParameters = function () { return [
        { type: _translation_translation_service__WEBPACK_IMPORTED_MODULE_4__["TranslateService"] },
        { type: _angular_material_snack_bar__WEBPACK_IMPORTED_MODULE_5__["MatSnackBar"] },
        { type: _angular_core__WEBPACK_IMPORTED_MODULE_3__["NgZone"] },
        { type: angular2_hotkeys__WEBPACK_IMPORTED_MODULE_6__["HotkeysService"] }
    ]; };
    AppComponent = Object(tslib__WEBPACK_IMPORTED_MODULE_0__["__decorate"])([
        Object(_angular_core__WEBPACK_IMPORTED_MODULE_3__["Component"])({
            selector: 'app-root',
            template: _raw_loader_app_component_html__WEBPACK_IMPORTED_MODULE_1__["default"],
            styles: [_app_component_scss__WEBPACK_IMPORTED_MODULE_2__["default"]]
        }),
        Object(tslib__WEBPACK_IMPORTED_MODULE_0__["__metadata"])("design:paramtypes", [_translation_translation_service__WEBPACK_IMPORTED_MODULE_4__["TranslateService"],
            _angular_material_snack_bar__WEBPACK_IMPORTED_MODULE_5__["MatSnackBar"],
            _angular_core__WEBPACK_IMPORTED_MODULE_3__["NgZone"],
            angular2_hotkeys__WEBPACK_IMPORTED_MODULE_6__["HotkeysService"]])
    ], AppComponent);
    return AppComponent;
}());



/***/ }),

/***/ "T7HD":
/*!***********************************!*\
  !*** ./src/app/translation/fr.ts ***!
  \***********************************/
/*! exports provided: LANG_FR_NAME, LANG_FR_TRANS */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "LANG_FR_NAME", function() { return LANG_FR_NAME; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "LANG_FR_TRANS", function() { return LANG_FR_TRANS; });
/*
Copyright (c) 2016 Álan Crístoffer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
// tslint:disable:max-line-length
var LANG_FR_NAME = 'fr';
var LANG_FR_TRANS = {
    'English': 'Anglais',
    'French': 'Français',
    'German': 'Allemand',
    'Portuguese': 'Portugais',
    'Language': 'Langue',
    'Password': 'Mot de passe',
    'Create': 'Créer',
    'Open': 'Ouvrir',
    'Cancel': 'Annuler',
    'The selected folder is empty. To create a Store, choose Create.': 'Le dossier sélectionné est vide. Pour créer un Store, choisissez Créer.',
    'Wrong password or key.': 'Mot de passe ou clé faux.',
    'Could not open the Store. Check file permissions.': 'Impossible d\'ouvrir le Store. Vérifiez les autorisations de fichier.',
    'Could not open file. Check file permissions.': 'Impossible d\'ouvrir le fichier. Vérifiez les autorisations de fichier.',
    'Could not write to file. Check file permissions.': 'Vous ne pouvez pas écrire dans le fichier. Vérifiez les autorisations de fichier.',
    'Selected file Could not be processed: too big.': 'Le fichier sélectionné ne peut pas être traitée: trop gros.',
    'Selected file does not exist.': 'Le fichier sélectionné ne existe pas.',
    'File part appears to be corrupted.': 'Une partie du fichier semble être endommagé.',
    'Wrong file checksum (i.e.: not the expected content).': 'Checksum de fichier incorrect (pas le contenu attendu).',
    'File already exists.': 'Le fichier existe déjà.',
    'Password should have at least 4 characters.': 'Le mot de passe doit comporter au moins 4 caractères.'
};


/***/ }),

/***/ "VzVu":
/*!**************************************************************************!*\
  !*** ./node_modules/raw-loader/dist/cjs.js!./src/app/app.component.html ***!
  \**************************************************************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony default export */ __webpack_exports__["default"] = ("<mat-card class=\"example-card\">\n  <img id=\"logo\" mat-card-image src=\"assets/imgs/icon.svg\">\n  <mat-card-content>\n    <mat-form-field *ngIf=\"!!storePath\">\n      <input autofocus matInput placeholder=\"{{ 'Password' | translate}}\" type=\"password\" [(ngModel)]=\"password\"\n        (keyup.enter)=\"open()\">\n    </mat-form-field>\n  </mat-card-content>\n  <mat-card-actions>\n    <button (click)=\"select()\" class=\"w50\" mat-button color=\"primary\" *ngIf=\"!storePath\">{{ 'Open' | translate }}</button>\n    <button (click)=\"create()\" class=\"w50\" mat-button color=\"primary\" *ngIf=\"!storePath\">{{ 'Create' | translate }}</button>\n    <button (click)=\"open()\" class=\"w50\" mat-button color=\"primary\" *ngIf=\"!!storePath\">{{ 'Open' | translate }}</button>\n    <button (click)=\"cancel()\" class=\"w50\" mat-button color=\"primary\" *ngIf=\"!!storePath\">{{ 'Cancel' | translate }}</button>\n  </mat-card-actions>\n</mat-card>\n\n<div id=\"flags\">\n  <a (click)=\"setLanguage('en')\"><img class=\"flag\" src=\"assets/imgs/en.svg\"></a>\n  <a (click)=\"setLanguage('de')\"><img class=\"flag\" src=\"assets/imgs/de.svg\"></a>\n  <a (click)=\"setLanguage('fr')\"><img class=\"flag\" src=\"assets/imgs/fr.svg\"></a>\n  <a (click)=\"setLanguage('pt')\"><img class=\"flag\" src=\"assets/imgs/pt.svg\"></a>\n</div>\n");

/***/ }),

/***/ "ZAI4":
/*!*******************************!*\
  !*** ./src/app/app.module.ts ***!
  \*******************************/
/*! exports provided: AppModule */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "AppModule", function() { return AppModule; });
/* harmony import */ var tslib__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! tslib */ "mrSG");
/* harmony import */ var _angular_platform_browser__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @angular/platform-browser */ "jhN1");
/* harmony import */ var _angular_core__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @angular/core */ "fXoL");
/* harmony import */ var _app_component__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ./app.component */ "Sy1n");
/* harmony import */ var _angular_platform_browser_animations__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! @angular/platform-browser/animations */ "R1ws");
/* harmony import */ var _translation_translation_pipe__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! ./translation/translation.pipe */ "kJAI");
/* harmony import */ var _translation_translation__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! ./translation/translation */ "jroB");
/* harmony import */ var _translation_translation_service__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(/*! ./translation/translation.service */ "9fjc");
/* harmony import */ var _angular_forms__WEBPACK_IMPORTED_MODULE_8__ = __webpack_require__(/*! @angular/forms */ "3Pt+");
/* harmony import */ var angular2_hotkeys__WEBPACK_IMPORTED_MODULE_9__ = __webpack_require__(/*! angular2-hotkeys */ "yl57");
/* harmony import */ var _angular_material_button__WEBPACK_IMPORTED_MODULE_10__ = __webpack_require__(/*! @angular/material/button */ "bTqV");
/* harmony import */ var _angular_material_card__WEBPACK_IMPORTED_MODULE_11__ = __webpack_require__(/*! @angular/material/card */ "Wp6s");
/* harmony import */ var _angular_material_form_field__WEBPACK_IMPORTED_MODULE_12__ = __webpack_require__(/*! @angular/material/form-field */ "kmnG");
/* harmony import */ var _angular_material_input__WEBPACK_IMPORTED_MODULE_13__ = __webpack_require__(/*! @angular/material/input */ "qFsG");
/* harmony import */ var _angular_material_snack_bar__WEBPACK_IMPORTED_MODULE_14__ = __webpack_require__(/*! @angular/material/snack-bar */ "dNgK");
/* harmony import */ var _angular_material_toolbar__WEBPACK_IMPORTED_MODULE_15__ = __webpack_require__(/*! @angular/material/toolbar */ "/t3+");
















var AppModule = /** @class */ (function () {
    function AppModule() {
    }
    AppModule = Object(tslib__WEBPACK_IMPORTED_MODULE_0__["__decorate"])([
        Object(_angular_core__WEBPACK_IMPORTED_MODULE_2__["NgModule"])({
            declarations: [
                _app_component__WEBPACK_IMPORTED_MODULE_3__["AppComponent"],
                _translation_translation_pipe__WEBPACK_IMPORTED_MODULE_5__["TranslatePipe"]
            ],
            imports: [
                _angular_platform_browser__WEBPACK_IMPORTED_MODULE_1__["BrowserModule"],
                _angular_platform_browser_animations__WEBPACK_IMPORTED_MODULE_4__["BrowserAnimationsModule"],
                _angular_forms__WEBPACK_IMPORTED_MODULE_8__["FormsModule"],
                _angular_material_button__WEBPACK_IMPORTED_MODULE_10__["MatButtonModule"],
                _angular_material_card__WEBPACK_IMPORTED_MODULE_11__["MatCardModule"],
                _angular_material_form_field__WEBPACK_IMPORTED_MODULE_12__["MatFormFieldModule"],
                _angular_material_input__WEBPACK_IMPORTED_MODULE_13__["MatInputModule"],
                _angular_material_snack_bar__WEBPACK_IMPORTED_MODULE_14__["MatSnackBarModule"],
                _angular_material_toolbar__WEBPACK_IMPORTED_MODULE_15__["MatToolbarModule"],
                angular2_hotkeys__WEBPACK_IMPORTED_MODULE_9__["HotkeyModule"].forRoot()
            ],
            providers: [
                _translation_translation__WEBPACK_IMPORTED_MODULE_6__["TRANSLATION_PROVIDERS"],
                _translation_translation_service__WEBPACK_IMPORTED_MODULE_7__["TranslateService"]
            ],
            bootstrap: [_app_component__WEBPACK_IMPORTED_MODULE_3__["AppComponent"]]
        })
    ], AppModule);
    return AppModule;
}());



/***/ }),

/***/ "aRlq":
/*!***********************************!*\
  !*** ./src/app/translation/de.ts ***!
  \***********************************/
/*! exports provided: LANG_DE_NAME, LANG_DE_TRANS */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "LANG_DE_NAME", function() { return LANG_DE_NAME; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "LANG_DE_TRANS", function() { return LANG_DE_TRANS; });
/*
Copyright (c) 2016 Álan Crístoffer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
// tslint:disable:max-line-length
var LANG_DE_NAME = 'de';
var LANG_DE_TRANS = {
    'English': 'Englisch',
    'French': 'Französisch',
    'German': 'Deutsch',
    'Portuguese': 'Portuguiesisch',
    'Language': 'Sprache',
    'Password': 'Passwort',
    'Create': 'Erstellen',
    'Open': 'Öffnen',
    'Cancel': 'Abbrechen',
    'The selected folder is empty. To create a Store, choose Create.': 'Der gewählte Ordner ist leer. Um ein Store zu erzeugen, klicken Sie Erzeugen.',
    'Wrong password or key.': 'Passwort oder Schlüssel ist falsch.',
    'Could not open the Store. Check file permissions.': 'Store kann nicht geöffnet werden. Sehen Sie die Dateiberechtigungen.',
    'Could not open file. Check file permissions.': 'Eine Datei kann nicht geöffnet sein. Sehen Sie die Dateiberechtigungen.',
    'Could not write to file. Check file permissions.': 'Eine Datei kann nicht geschrieben sein. Sehen Sie die Dateiberechtigungen.',
    'Selected file Could not be processed: too big.': 'Die gewählte Datei kann nicht verarbeitet sein: zu groß.',
    'Selected file does not exist.': 'Die gewählte Datei existiert nicht.',
    'File part appears to be corrupted.': 'Dateipart scheint korrupt zu sein.',
    'Wrong file checksum (i.e.: not the expected content).': 'Falsches Checksum (Inhalt ist nicht was erwartet war).',
    'File already exists.': 'Die Datei existiert bereits.',
    'Password should have at least 4 characters.': 'Das Passwort sollte mindestens 4 Zeichen haben.'
};


/***/ }),

/***/ "crnd":
/*!**********************************************************!*\
  !*** ./src/$$_lazy_route_resource lazy namespace object ***!
  \**********************************************************/
/*! no static exports found */
/***/ (function(module, exports) {

function webpackEmptyAsyncContext(req) {
	// Here Promise.resolve().then() is used instead of new Promise() to prevent
	// uncaught exception popping up in devtools
	return Promise.resolve().then(function() {
		var e = new Error("Cannot find module '" + req + "'");
		e.code = 'MODULE_NOT_FOUND';
		throw e;
	});
}
webpackEmptyAsyncContext.keys = function() { return []; };
webpackEmptyAsyncContext.resolve = webpackEmptyAsyncContext;
module.exports = webpackEmptyAsyncContext;
webpackEmptyAsyncContext.id = "crnd";

/***/ }),

/***/ "jroB":
/*!********************************************!*\
  !*** ./src/app/translation/translation.ts ***!
  \********************************************/
/*! exports provided: TRANSLATIONS, Dictionary, TRANSLATION_PROVIDERS */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TRANSLATIONS", function() { return TRANSLATIONS; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "Dictionary", function() { return Dictionary; });
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TRANSLATION_PROVIDERS", function() { return TRANSLATION_PROVIDERS; });
/* harmony import */ var _angular_core__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! @angular/core */ "fXoL");
/* harmony import */ var _de__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./de */ "aRlq");
/* harmony import */ var _fr__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./fr */ "T7HD");
/* harmony import */ var _pt__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ./pt */ "0hYy");
/*
Copyright (c) 2016 Álan Crístoffer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/




var TRANSLATIONS = new _angular_core__WEBPACK_IMPORTED_MODULE_0__["InjectionToken"]('translations');
var Dictionary = /** @class */ (function () {
    function Dictionary() {
        var _a;
        this.dictionary = (_a = {},
            _a[_de__WEBPACK_IMPORTED_MODULE_1__["LANG_DE_NAME"]] = _de__WEBPACK_IMPORTED_MODULE_1__["LANG_DE_TRANS"],
            _a[_fr__WEBPACK_IMPORTED_MODULE_2__["LANG_FR_NAME"]] = _fr__WEBPACK_IMPORTED_MODULE_2__["LANG_FR_TRANS"],
            _a[_pt__WEBPACK_IMPORTED_MODULE_3__["LANG_PT_NAME"]] = _pt__WEBPACK_IMPORTED_MODULE_3__["LANG_PT_TRANS"],
            _a);
    }
    Dictionary.prototype.get = function (language) {
        return this.dictionary[language];
    };
    return Dictionary;
}());

var TRANSLATION_PROVIDERS = [
    { provide: TRANSLATIONS, useClass: Dictionary },
];


/***/ }),

/***/ "kJAI":
/*!*************************************************!*\
  !*** ./src/app/translation/translation.pipe.ts ***!
  \*************************************************/
/*! exports provided: TranslatePipe */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, "TranslatePipe", function() { return TranslatePipe; });
/* harmony import */ var tslib__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! tslib */ "mrSG");
/* harmony import */ var _angular_core__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @angular/core */ "fXoL");
/* harmony import */ var _translation_service__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./translation.service */ "9fjc");
/*
Copyright (c) 2016 Álan Crístoffer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/



var TranslatePipe = /** @class */ (function () {
    function TranslatePipe(_translate) {
        this._translate = _translate;
    }
    TranslatePipe.prototype.transform = function (value, _) {
        if (!value) {
            return;
        }
        return this._translate.instant(value);
    };
    TranslatePipe.ctorParameters = function () { return [
        { type: _translation_service__WEBPACK_IMPORTED_MODULE_2__["TranslateService"] }
    ]; };
    TranslatePipe = Object(tslib__WEBPACK_IMPORTED_MODULE_0__["__decorate"])([
        Object(_angular_core__WEBPACK_IMPORTED_MODULE_1__["Pipe"])({
            name: 'translate',
            pure: false
        }),
        Object(tslib__WEBPACK_IMPORTED_MODULE_0__["__metadata"])("design:paramtypes", [_translation_service__WEBPACK_IMPORTED_MODULE_2__["TranslateService"]])
    ], TranslatePipe);
    return TranslatePipe;
}());



/***/ }),

/***/ "ynWL":
/*!************************************!*\
  !*** ./src/app/app.component.scss ***!
  \************************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony default export */ __webpack_exports__["default"] = ("mat-card {\n  margin: 1rem;\n}\nmat-card #logo {\n  display: block;\n  width: 120px;\n  height: auto;\n  margin: 0 auto;\n}\nmat-card .w50 {\n  width: 45%;\n}\nmat-card .w100 {\n  width: 100%;\n}\nmat-card mat-card-content {\n  width: 300px;\n}\nmat-card mat-form-field {\n  width: 100%;\n  margin-top: 1rem 0;\n}\n#flags {\n  text-align: center;\n}\n#flags .flag {\n  margin: 0 0.5rem;\n  width: 40px;\n  height: auto;\n  cursor: pointer;\n}\n/*# sourceMappingURL=data:application/json;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbInNyYy9hcHAvYXBwLmNvbXBvbmVudC5zY3NzIl0sIm5hbWVzIjpbXSwibWFwcGluZ3MiOiJBQUFBO0VBQ0UsWUFBQTtBQUNGO0FBQ0U7RUFDRSxjQUFBO0VBQ0EsWUFBQTtFQUNBLFlBQUE7RUFDQSxjQUFBO0FBQ0o7QUFFRTtFQUNFLFVBQUE7QUFBSjtBQUdFO0VBQ0UsV0FBQTtBQURKO0FBSUU7RUFDRSxZQUFBO0FBRko7QUFLRTtFQUNFLFdBQUE7RUFDQSxrQkFBQTtBQUhKO0FBT0E7RUFDRSxrQkFBQTtBQUpGO0FBTUU7RUFDRSxnQkFBQTtFQUNBLFdBQUE7RUFDQSxZQUFBO0VBQ0EsZUFBQTtBQUpKIiwiZmlsZSI6InNyYy9hcHAvYXBwLmNvbXBvbmVudC5zY3NzIiwic291cmNlc0NvbnRlbnQiOlsibWF0LWNhcmQge1xuICBtYXJnaW46IDFyZW07XG5cbiAgI2xvZ28ge1xuICAgIGRpc3BsYXk6IGJsb2NrO1xuICAgIHdpZHRoOiAxMjBweDtcbiAgICBoZWlnaHQ6IGF1dG87XG4gICAgbWFyZ2luOiAwIGF1dG87XG4gIH1cblxuICAudzUwIHtcbiAgICB3aWR0aDogNDUlO1xuICB9XG5cbiAgLncxMDAge1xuICAgIHdpZHRoOiAxMDAlO1xuICB9XG5cbiAgbWF0LWNhcmQtY29udGVudCB7XG4gICAgd2lkdGg6IDMwMHB4O1xuICB9XG5cbiAgbWF0LWZvcm0tZmllbGQge1xuICAgIHdpZHRoOiAxMDAlO1xuICAgIG1hcmdpbi10b3A6IDFyZW0gMDtcbiAgfVxufVxuXG4jZmxhZ3Mge1xuICB0ZXh0LWFsaWduOiBjZW50ZXI7XG5cbiAgLmZsYWcge1xuICAgIG1hcmdpbjogMCAwLjVyZW07XG4gICAgd2lkdGg6IDQwcHg7XG4gICAgaGVpZ2h0OiBhdXRvO1xuICAgIGN1cnNvcjogcG9pbnRlcjtcbiAgfVxufVxuIl19 */");

/***/ }),

/***/ "zUnb":
/*!*********************!*\
  !*** ./src/main.ts ***!
  \*********************/
/*! no exports provided */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
__webpack_require__.r(__webpack_exports__);
/* harmony import */ var hammerjs__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! hammerjs */ "yLV6");
/* harmony import */ var hammerjs__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(hammerjs__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var _angular_core__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! @angular/core */ "fXoL");
/* harmony import */ var _angular_platform_browser_dynamic__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! @angular/platform-browser-dynamic */ "a3Wg");
/* harmony import */ var _app_app_module__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ./app/app.module */ "ZAI4");
/* harmony import */ var _environments_environment__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! ./environments/environment */ "AytR");





var _qwc = new QWebChannel(qt.webChannelTransport, function (channel) {
    window.bridge = channel.objects.welcome_bridge;
    if (_environments_environment__WEBPACK_IMPORTED_MODULE_4__["environment"].production) {
        Object(_angular_core__WEBPACK_IMPORTED_MODULE_1__["enableProdMode"])();
    }
    Object(_angular_platform_browser_dynamic__WEBPACK_IMPORTED_MODULE_2__["platformBrowserDynamic"])().bootstrapModule(_app_app_module__WEBPACK_IMPORTED_MODULE_3__["AppModule"]).catch(console.error);
});


/***/ })

},[[0,"runtime","vendor"]]]);
//# sourceMappingURL=main.js.map