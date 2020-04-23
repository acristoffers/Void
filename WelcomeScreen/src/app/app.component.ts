import { Component, OnInit, NgZone } from '@angular/core';
import { TranslateService } from './translation/translation.service';
import { MatSnackBar } from '@angular/material/snack-bar';
import { HotkeysService, Hotkey } from 'angular2-hotkeys';

declare class Bridge {
  getExisting(result: (result: boolean) => void): void;
  getNew(result: (result: boolean) => void): void;
  open(password: string, result: (result: string) => void): void;
  create(password: string, result: (result: string) => void): void;
  close(): void;
  setLang(lang: string): void;
  lang(result: (result: string) => void): void;
}

declare let bridge: Bridge;

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  isNew = false;
  storePath = false;
  password = '';

  constructor(
    private translate: TranslateService,
    private toast: MatSnackBar,
    private zone: NgZone,
    private hotkeys: HotkeysService
  ) {
    this.hotkeys.add(new Hotkey(['meta+o', 'ctrl+o'], (_: KeyboardEvent): boolean => {
      this.select();
      return false;
    }));

    this.hotkeys.add(new Hotkey(['meta+n', 'ctrl+n'], (_: KeyboardEvent): boolean => {
      this.create();
      return false;
    }));
  }

  ngOnInit(): void {
    bridge.lang(lang => {
      this.zone.run(() => {
        this.translate.use(lang || 'en');
      });
    });
  }

  setLanguage(lang: string): void {
    bridge.setLang(lang);
    this.translate.use(lang);
  }

  create(): void {
    bridge.getNew(path => {
      this.zone.run(() => {
        this.storePath = path;
        this.isNew = true;
      });
    });
  }

  open(): void {
    if (this.password.trim().length < 4) {
      const msg = this.translate.instant('Password should have at least 4 characters.');
      this.toast.open(msg, null, { duration: 2000 });
      return;
    }

    const action = this.isNew ? bridge.create : bridge.open;
    action(this.password, result => {
      this.zone.run(() => {
        if (result === 'Success') {
          bridge.close();
        } else {
          const messages = {
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

          const msg = this.translate.instant(messages[result]);
          this.toast.open(msg, null, { duration: 2000 });
        }
      });
    });
  }

  select(): void {
    bridge.getExisting(path => {
      this.zone.run(() => {
        this.storePath = path;
        this.isNew = false;
      });
    });
  }

  cancel(): void {
    this.storePath = false;
    this.password = '';
  }
}
