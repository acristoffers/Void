import { Component } from '@angular/core';
import { MatIconRegistry } from '@angular/material';
import { TranslateService } from '../translation';
import { DomSanitizer } from '@angular/platform-browser';
import { BridgeService } from '../bridge.service';

import * as _ from 'lodash';

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

  constructor(
    private iconRegistry: MatIconRegistry,
    private sanitizer: DomSanitizer,
    private translate: TranslateService,
    private bridge: BridgeService
  ) {
    this.registerImages();
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
}
