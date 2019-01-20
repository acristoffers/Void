import * as _ from 'lodash';

import { Component, Input, Output, EventEmitter } from '@angular/core';
import { MatIconRegistry, MatDialog } from '@angular/material';
import { TranslateService } from '../translation';
import { DomSanitizer } from '@angular/platform-browser';
import { BridgeService } from '../bridge.service';
import { SearchDialogComponent } from '../search-dialog/search-dialog.component';
import { HotkeysService, Hotkey } from 'angular2-hotkeys';

@Component({
  selector: 'app-toolbar',
  templateUrl: './toolbar.component.html',
  styleUrls: ['./toolbar.component.scss']
})
export class ToolbarComponent {
  enFlagUrl = 'assets/imgs/en.svg';
  deFlagUrl = 'assets/imgs/de.svg';
  frFlagUrl = 'assets/imgs/fr.svg';
  ptFlagUrl = 'assets/imgs/pt.svg';

  currentPath: string;

  @Input()
  get path() {
    return this.currentPath;
  }

  @Output()
  pathChange = new EventEmitter();

  set path(p: string) {
    this.currentPath = p;
    this.pathChange.emit(p);
  }

  constructor(
    private iconRegistry: MatIconRegistry,
    private sanitizer: DomSanitizer,
    private translate: TranslateService,
    private bridge: BridgeService,
    private dialog: MatDialog,
    private hotkeys: HotkeysService
  ) {
    this.registerImages();

    this.hotkeys.add(new Hotkey('meta+f', (event: KeyboardEvent): boolean => {
      this.search();
      return false;
    }));
  }

  registerImages(): void {
    const images = {
      'app-icon': 'assets/imgs/icon.svg',
      'en-flag': this.enFlagUrl,
      'fr-flag': this.frFlagUrl,
      'de-flag': this.deFlagUrl,
      'pt-flag': this.ptFlagUrl,
    };

    _.map(images, (url: string, key: string) => {
      const safeUrl = this.sanitizer.bypassSecurityTrustResourceUrl(url);
      this.iconRegistry.addSvgIcon(key, safeUrl);
    });
  }

  setLanguage(lang: string): void {
    this.translate.use(lang);
    this.bridge.setLang(lang);
  }

  search() {
    this.dialog.open(SearchDialogComponent).afterClosed().subscribe((path: string) => {
      if (path != null) {
        this.path = path;
      }
    });
  }
}
