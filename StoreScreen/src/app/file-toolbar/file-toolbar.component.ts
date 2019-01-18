import { Component, EventEmitter, Input, Output } from '@angular/core';
import { MatDialog, MatSnackBar } from '@angular/material';
import { Hotkey, HotkeysService } from 'angular2-hotkeys';
import { BridgeService } from '../bridge.service';
import { InputDialogComponent, InputDialogData } from '../input-dialog/input-dialog.component';
import { TranslateService } from '../translation';

@Component({
  selector: 'app-file-toolbar',
  templateUrl: './file-toolbar.component.html',
  styleUrls: ['./file-toolbar.component.scss']
})
export class FileToolbarComponent {
  currentPath: string;
  currentDisplay: string;
  currentInfoOpen: boolean;

  constructor(
    private dialog: MatDialog,
    private translate: TranslateService,
    private bridge: BridgeService,
    private toast: MatSnackBar,
    private hotkeys: HotkeysService
  ) {
    this.hotkeys.add(new Hotkey('meta+i', (event: KeyboardEvent): boolean => {
      this.infoOpen = !this.infoOpen;
      return false;
    }, null, () => this.translate.instant('Toggle information panel.')));

    this.hotkeys.add(new Hotkey('meta+g', (event: KeyboardEvent): boolean => {
      this.toggleDisplay();
      return false;
    }, null, () => this.translate.instant('Toggle grid/list.')));
  }

  @Input()
  get path() {
    return this.currentPath;
  }

  set path(p: string) {
    this.currentPath = p;
    this.pathChange.emit(p);
  }

  @Input()
  get display() {
    return this.currentDisplay;
  }

  set display(p: string) {
    this.currentDisplay = p;
    this.displayChange.emit(p);
  }

  @Input()
  get infoOpen() {
    return this.currentInfoOpen;
  }

  set infoOpen(p: boolean) {
    this.currentInfoOpen = p;
    this.infoOpenChange.emit(p);
  }

  @Output()
  pathChange = new EventEmitter();

  @Output()
  displayChange = new EventEmitter();

  @Output()
  infoOpenChange = new EventEmitter();

  toggleDisplay() {
    this.display = (this.display === 'grid' ? 'list' : 'grid');
  }

  toggleInfo() {
    this.infoOpen = !this.infoOpen;
  }

  newFile() {
    const data: InputDialogData = {
      title: this.translate.instant('Add File'),
      value: '',
      type: 'text'
    };

    this.dialog.open(InputDialogComponent, { data: data })
      .afterClosed()
      .subscribe(name => {
        const path = this.bridge.appendPath(this.currentPath, name);

        if (path.endsWith('/')) {
          const msg = this.translate.instant('Invalid filename');
          this.toast.open(msg, null, { duration: 2000 });
          return;
        }

        this.bridge.createFile(path).subscribe();
      });
  }

  newFolder() {
    const data: InputDialogData = {
      title: this.translate.instant('Add File'),
      value: '',
      type: 'text'
    };

    this.dialog.open(InputDialogComponent, { data: data })
      .afterClosed()
      .subscribe(name => {
        const path = this.bridge.appendPath(this.currentPath, name);

        if (path.endsWith('/')) {
          const msg = this.translate.instant('Invalid filename');
          this.toast.open(msg, null, { duration: 2000 });
          return;
        }

        this.bridge.createDir(path).subscribe();
      });
  }

  addFile() {
    this.bridge.addFile(this.currentPath);
  }

  addFolder() {
    this.bridge.addFolder(this.currentPath);
  }
}
