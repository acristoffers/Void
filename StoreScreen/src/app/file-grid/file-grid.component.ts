import { Component, EventEmitter, Input, NgZone, Output } from '@angular/core';
import { MatDialog, MatMenuTrigger, MatSnackBar } from '@angular/material';
import { DomSanitizer, SafeUrl } from '@angular/platform-browser';
import { Hotkey, HotkeysService } from 'angular2-hotkeys';
import * as _ from 'lodash';
import { filter } from 'rxjs/operators';
import { BridgeService, FileNode } from '../bridge.service';
import { ImageViewerComponent } from '../image-viewer/image-viewer.component';
import { InputDialogComponent, InputDialogData } from '../input-dialog/input-dialog.component';
import { TextEditorComponent, TextEditorData } from '../text-editor/text-editor.component';
import { TranslateService } from '../translation/translation.service';

// tslint:disable:max-line-length

@Component({
  selector: 'app-file-grid',
  templateUrl: './file-grid.component.html',
  styleUrls: ['./file-grid.component.scss']
})
export class FileGridComponent {
  currentPath: string;
  rootNode: FileNode;
  currentNode: FileNode;
  _selection: string[] = [];
  cursor = 0;
  shift = false;
  ctrl = false;
  dropPath = '';

  get selection(): string[] {
    return this._selection;
  }

  set selection(ps: string[]) {
    this._selection = ps;

    if (this.currentNode != null) {
      BridgeService.fileInfo.next(_.first(_.filter(this.currentNode.children, c => c.path === _.first(ps))));
    }
  }

  @Input() display = 'grid';

  @Input()
  get path() {
    return this.currentPath;
  }

  @Output()
  pathChange = new EventEmitter();

  set path(p: string) {
    this.currentPath = p;
    this.pathChange.emit(p);
    this.updateCurrentNode();

    this.cursor = 0;
    if (this.currentNode && this.currentNode.children.length > 0) {
      this.selection = [this.sorted(this.currentNode)[this.cursor].path];
    } else {
      this.selection = [];
    }
  }

  setPath(path: string) {
    this.path = path;
  }

  constructor(
    private bridge: BridgeService,
    private zone: NgZone,
    private sanitizer: DomSanitizer,
    private translate: TranslateService,
    private toast: MatSnackBar,
    private hotkeys: HotkeysService,
    private dialog: MatDialog
  ) {
    this.bridge.fileTreeObservable().subscribe(tree => {
      this.zone.run(() => {
        this.rootNode = tree;
        this.cursor = 0;
        this.updateCurrentNode();

        if (this.currentNode && this.currentNode.children.length > 0) {
          this.selection = [this.sorted(this.currentNode)[this.cursor].path];
        } else {
          this.selection = [];
        }
      });
    });

    BridgeService.keyPressedSubject.pipe(filter(key => key === 'left')).subscribe(__ => {
      this.shift = false;
      this.cursorLeft();
    });

    BridgeService.keyPressedSubject.pipe(filter(key => key === 'right')).subscribe(__ => {
      this.shift = false;
      this.cursorRight();
    });

    BridgeService.keyPressedSubject.pipe(filter(key => key === 'esc')).subscribe(__ => {
      this.shift = false;
      this.cursor = 0;
      if (this.currentNode.children.length > 0) {
        this.selection = [this.sorted(this.currentNode)[this.cursor].path];
      } else {
        this.selection = [];
      }
    });

    this.hotkeys.add(new Hotkey(['backspace', 'delete'], (event: KeyboardEvent): boolean => {
      this.remove();
      return false;
    }));

    this.hotkeys.add(new Hotkey('enter', (event: KeyboardEvent): boolean => {
      this.open(_.first(_.filter(this.currentNode.children, i => i.path === _.first(this.selection))));
      return false;
    }));

    this.hotkeys.add(new Hotkey('meta+a', (event: KeyboardEvent): boolean => {
      this.shift = false;
      this.selection = _.map(this.currentNode.children, 'path');
      return false;
    }));

    this.hotkeys.add(new Hotkey('up', (event: KeyboardEvent): boolean => {
      this.shift = false;
      this.cursorUp();
      return false;
    }));

    this.hotkeys.add(new Hotkey('down', (event: KeyboardEvent): boolean => {
      this.shift = false;
      this.cursorDown();
      return false;
    }));

    this.hotkeys.add(new Hotkey('shift+left', (event: KeyboardEvent): boolean => {
      this.shift = true;
      this.cursorLeft();
      return false;
    }));

    this.hotkeys.add(new Hotkey('shift+right', (event: KeyboardEvent): boolean => {
      this.shift = true;
      this.cursorRight();
      return false;
    }));

    this.hotkeys.add(new Hotkey('shift+up', (event: KeyboardEvent): boolean => {
      this.shift = true;
      this.cursorUp();
      return false;
    }));

    this.hotkeys.add(new Hotkey('shift+down', (event: KeyboardEvent): boolean => {
      this.shift = true;
      this.cursorDown();
      return false;
    }));

    window.addEventListener('keyup', (key: KeyboardEvent) => {
      if (key.ctrlKey || key.metaKey) {
        this.ctrl = false;
      }

      if (key.shiftKey) {
        this.shift = false;
      }
    });

    window.addEventListener('keydown', (key: KeyboardEvent) => {
      if (key.ctrlKey || key.metaKey) {
        this.ctrl = true;
      }

      if (key.shiftKey) {
        this.shift = true;
      }
    });
  }

  sorted(node: FileNode): FileNode[] {
    if (node == null) {
      return [];
    } else {
      return _.sortBy(node.children, ['type', 'name']);
    }
  }

  iconFor(node: FileNode): string {
    const mimetype = node.type;
    if (mimetype === 'inode/directory') {
      return 'folder';
    } else if (mimetype.startsWith('video')) {
      return 'movie';
    } else if (mimetype.startsWith('audio')) {
      return 'audiotrack';
    } else if (this.isText(node)) {
      return 'edit';
    } else {
      return 'insert_drive_file';
    }
  }

  urlFor(node: FileNode): SafeUrl {
    const url = `thumb://${node.path}`;
    return this.sanitizer.bypassSecurityTrustUrl(url);
  }

  classFor(node: FileNode): string {
    let className = 'card';
    className += this.display === 'grid' ? '-grid' : '-list';
    className += node.type.startsWith('image') ? '-image' : '-icon';
    return className;
  }

  openMenu(event: MouseEvent, viewChild: MatMenuTrigger, node: FileNode) {
    if (!_.includes(this.selection, node.path)) {
      this.clickOn(node);
    }

    event.preventDefault();
    viewChild.openMenu();
  }

  open(node: FileNode) {
    if (node.type.startsWith('inode')) {
      this.setPath(node.path);
    } else if (node.type.startsWith('image') || node.type.startsWith('video')) {
      this.view(node);
    } else if (this.isText(node)) {
      this.edit(node);
    }
  }

  decrypt() {
    this.bridge.decrypt(this.selection, this.currentNode.path).subscribe();
  }

  view(node: FileNode) {
    if (node.type.startsWith('image')) {
      ImageViewerComponent.images.next(_.map(_.filter(this.sorted(this.currentNode), n => n.type.startsWith('image')), 'path'));
      ImageViewerComponent.show.next(true);
      ImageViewerComponent.setCurrent.next(node.path);
    } else if (node.type.startsWith('video')) {
      this.bridge.playVideo(node).subscribe();
    }
  }

  edit(node: FileNode) {
    const data: TextEditorData = {
      filePath: node.path,
      mimetype: node.type
    };

    this.dialog.open(TextEditorComponent, { data: data });
  }

  remove() {
    const msg = this.translate.instant('Are you sure that you want to delete this item?');
    if (confirm(msg)) {
      const baseName = (s: string) => _.last(s.split('/'));
      this.selection.forEach(path => {
        this.bridge.remove(path, false).subscribe(() => {
          BridgeService.statusChange.next({
            type: 'removeStart',
            path: path
          });
          setTimeout(() => {
            BridgeService.statusChange.next({
              type: 'removeEnd',
              path: path
            });
          }, 5000);
        });
      });
    }
  }

  rename(node: FileNode) {
    const data: InputDialogData = {
      title: this.translate.instant('Rename'),
      value: _.last(node.path.split('/')),
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

        this.bridge.move(node.path, path).subscribe();
      });
  }

  cursorLeft() {
    const oldItem = this.sorted(this.currentNode)[this.cursor].path;
    this.cursor = this.wrapAround(this.cursor - 1, this.currentNode.children.length);

    if (this.shift) {
      const item = this.sorted(this.currentNode)[this.cursor].path;
      if (_.includes(this.selection, item)) {
        this.selection = _.filter(this.selection, i => i !== oldItem);
      } else {
        this.selection = _.concat(this.selection, item);
      }
    } else {
      if (this.currentNode.children.length > 0) {
        this.selection = [this.sorted(this.currentNode)[this.cursor].path];
      } else {
        this.selection = [];
      }
    }
  }

  cursorRight() {
    const oldItem = this.sorted(this.currentNode)[this.cursor].path;
    this.cursor = this.wrapAround(this.cursor + 1, this.currentNode.children.length);

    if (this.shift) {
      const item = this.sorted(this.currentNode)[this.cursor].path;
      if (_.includes(this.selection, item)) {
        this.selection = _.filter(this.selection, i => i !== oldItem);
      } else {
        this.selection = _.concat(this.selection, item);
      }
    } else {
      if (this.currentNode.children.length > 0) {
        this.selection = [this.sorted(this.currentNode)[this.cursor].path];
      } else {
        this.selection = [];
      }
    }
  }

  cursorUp() {
    this.cursorLeft();
    if (this.display === 'grid') {
      this.cursorLeft();
      this.cursorLeft();
    }
  }

  cursorDown() {
    this.cursorRight();
    if (this.display === 'grid') {
      this.cursorRight();
      this.cursorRight();
    }
  }

  clickOn(node: FileNode) {
    const oldCursor = this.cursor;
    this.cursor = _.findIndex(this.sorted(this.currentNode), { path: node.path });

    if (this.ctrl) {
      if (this.isSelected(node)) {
        this.selection = _.filter(this.selection, n => n !== node.path);
      } else {
        this.selection = _.concat(this.selection, this.sorted(this.currentNode)[this.cursor].path);
      }
    } else if (this.shift) {
      const diff = oldCursor - this.cursor;
      if (diff < 0) {
        this.cursor = oldCursor;
        _.range(-diff).forEach(() => this.cursorRight());
      } else {
        this.cursor = oldCursor;
        _.range(diff).forEach(() => this.cursorLeft());
      }
    } else {
      if (this.isSelected(node)) {
        this.selection = _.filter(this.selection, n => n === node.path);
      } else {
        this.selection = [this.sorted(this.currentNode)[this.cursor].path];
      }
    }
  }

  isSelected(node: FileNode): boolean {
    return _.includes(this.selection, node.path);
  }

  isText(node: FileNode): boolean {
    return node.type.startsWith('text') || TextEditorComponent.modeForMime(node.type) !== 'text_plain';
  }

  drag(event: DragEvent, node: FileNode) {
    if (!_.includes(this.selection, node.path)) {
      this.selection = [node.path];
    }

    event.dataTransfer.setData('paths', JSON.stringify(this.selection));
    this.selection.forEach(s => event.dataTransfer.setData(s, ''));
  }

  drop(event: DragEvent, node: FileNode) {
    if (node.type === 'inode/directory') {
      event.stopPropagation();
      event.preventDefault();
      this.dropPath = '';

      const fs: string[] = JSON.parse(event.dataTransfer.getData('paths') || '[]');
      if (!_.isEmpty(fs) && !_.includes(fs, node.path)) {
        fs.forEach(f => {
          const baseName = _.slice(f.split('/'), 0, -1).join('/') || '/';
          const to = f.replace(baseName, node.path);
          if (f !== to) {
            this.bridge.move(f, to).subscribe();
          }
        });
      }
    }
  }

  allowDrop(event: DragEvent, node: FileNode): boolean {
    if (node.type === 'inode/directory' && !_.includes(this.selection, node.path)) {
      event.preventDefault();
      return true;
    }

    return false;
  }

  dragEnter(event: DragEvent, node: FileNode) {
    if (node.type === 'inode/directory' && !_.includes(this.selection, node.path)) {
      this.dropPath = node.path;
    }
  }

  dragLeave(event: DragEvent, node: FileNode) {
    this.dropPath = '';
  }

  private wrapAround(v: number, m: number): number {
    return (v % m + m) % m;
  }

  private updateCurrentNode() {
    if (this.rootNode == null || this.currentPath == null) {
      return;
    }

    let node = this.rootNode;
    while (node.path !== this.currentPath) {
      node = _.first(_.filter(node.children, (child: FileNode) => this.currentPath.startsWith(child.path)));
      if (node == null) {
        this.path = _.slice(this.currentPath.split('/'), 0, -1).join('/') || '/';
        return;
      }
    }

    this.currentNode = node;
  }
}
