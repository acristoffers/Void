import { Component, OnInit, OnDestroy, Input } from '@angular/core';
import { Subscription } from 'rxjs';
import { HotkeysService, Hotkey } from 'angular2-hotkeys';

@Component({
  selector: 'app-hotkeys-cheatsheet',
  templateUrl: './hotkeys-cheatsheet.component.html',
  styleUrls: ['./hotkeys-cheatsheet.component.scss']
})
export class HotkeysCheatsheetComponent implements OnInit, OnDestroy {
  helpVisible = false;
  @Input() title = 'Keyboard Shortcuts:';
  subscription: Subscription;

  hotkeys: Hotkey[];

  constructor(private hotkeysService: HotkeysService) {
  }

  public ngOnInit(): void {
    this.subscription = this.hotkeysService.cheatSheetToggle.subscribe((isOpen) => {
      if (isOpen !== false) {
        this.hotkeys = this.hotkeysService.hotkeys.filter(hotkey => hotkey.description);
      }

      if (isOpen === false) {
        this.helpVisible = false;
      } else {
        this.toggleCheatSheet();
      }
    });
  }

  public ngOnDestroy(): void {
    if (this.subscription) {
      this.subscription.unsubscribe();
    }
  }

  public toggleCheatSheet(): void {
    this.helpVisible = !this.helpVisible;
  }

  public getDescription(hotkey: Hotkey) {
    if (hotkey.description instanceof Function) {
      return hotkey.description();
    } else {
      return hotkey.description;
    }
  }
}
