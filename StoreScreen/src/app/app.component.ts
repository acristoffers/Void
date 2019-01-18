import { Component } from '@angular/core';
import { BridgeService } from './bridge.service';
import { TranslateService } from './translation/translation.service';
import { HotkeysService, Hotkey } from 'angular2-hotkeys';

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
    private hotkeys: HotkeysService,
    private bridge: BridgeService,
    private translate: TranslateService
  ) {
    this.bridge.lang().subscribe(lang => {
      this.translate.use(lang);
    });

    this.hotkeys.add(new Hotkey('esc', (event: KeyboardEvent): boolean => {
      BridgeService.keyPressedSubject.next('esc');
      return false;
    }));

    this.hotkeys.add(new Hotkey('left', (event: KeyboardEvent): boolean => {
      BridgeService.keyPressedSubject.next('left');
      return false;
    }));

    this.hotkeys.add(new Hotkey('right', (event: KeyboardEvent): boolean => {
      BridgeService.keyPressedSubject.next('right');
      return false;
    }));
  }
}
