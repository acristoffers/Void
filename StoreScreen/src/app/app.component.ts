import { Component } from '@angular/core';
import { BridgeService, FileNode } from './bridge.service';
import { TranslateService } from './translation/translation.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  currentPath = '/';
  displayStyle = 'grid';
  infoOpen = true;

  constructor(
    private bridge: BridgeService,
    private translate: TranslateService
  ) {
    this.bridge.lang().subscribe(lang => {
      this.translate.use(lang);
    });
  }
}
