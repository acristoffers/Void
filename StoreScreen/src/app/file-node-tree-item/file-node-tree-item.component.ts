import { Component, EventEmitter, Input, NgZone, Output } from '@angular/core';
import { MatDialog, MatMenuTrigger, MatSnackBar } from '@angular/material';
import * as _ from 'lodash';
import { delay } from 'rxjs/operators';
import { BridgeService, FileNode } from '../bridge.service';
import { InputDialogComponent, InputDialogData } from '../input-dialog/input-dialog.component';
import { TranslateService } from '../translation';


@Component({
  selector: 'app-file-node-tree-item',
  templateUrl: './file-node-tree-item.component.html',
  styleUrls: ['./file-node-tree-item.component.scss']
})
export class FileNodeTreeItemComponent {
  isExpanded = false;
  currentPath = '/';

  @Input() node: FileNode = {
    children: [],
    name: 'Store',
    path: '/',
    type: 'inode/directory'
  };

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

  setPath(path: string) {
    this.path = path;
  }

  constructor(
    private bridge: BridgeService,
    private translate: TranslateService,
    private toast: MatSnackBar,
    private dialog: MatDialog,
    private zone: NgZone
  ) {
  }

  hasChildren(): boolean {
    return this.node && this.node.children && this.node.children.length !== 0 || false;
  }

  sorted(node: FileNode[]): FileNode[] {
    return _.sortBy(node, ['type', 'name']);
  }

  openMenu(event: MouseEvent, viewChild: MatMenuTrigger) {
    event.preventDefault();
    viewChild.openMenu();
  }

  decrypt() {
    const basePath = _.slice(this.node.path.split('/'), 0, -1).join('/');
    this.bridge.decrypt([this.node.path], basePath).subscribe();
  }

  rename() {
    const data: InputDialogData = {
      title: this.translate.instant('Rename'),
      value: _.last(this.node.path.split('/')),
      type: 'text'
    };

    this.dialog.open(InputDialogComponent, { data: data })
      .afterClosed()
      .subscribe(name => {
        const basePath = _.slice(this.node.path.split('/'), 0, -1).join('/');
        const newPath = this.bridge.appendPath(basePath, name);

        if (newPath.endsWith('/')) {
          const msg = this.translate.instant('Invalid filename');
          this.toast.open(msg, null, { duration: 2000 });
          return;
        }

        if (newPath === this.node.path) {
          return;
        }

        const updatePath = this.currentPath.startsWith(this.node.path);
        this.bridge.move(this.node.path, newPath).pipe(delay(500)).subscribe(() => {
          if (updatePath) {
            this.zone.run(() => this.setPath(newPath));
          }
        });
      });
  }

  remove() {
    const cmsg = this.translate.instant('Are you sure that you want to delete this item?');
    if (confirm(cmsg)) {
      if (this.currentPath === this.node.path) {
        const basePath = _.slice(this.node.path.split('/'), 0, -1).join('/');
        this.setPath(basePath);
      }

      this.bridge.remove(this.node.path, false).subscribe(() => {
        BridgeService.statusChange.next({
          type: 'removeStart',
          path: this.node.path
        });
        setTimeout(() => {
          BridgeService.statusChange.next({
            type: 'removeEnd',
            path: this.node.path
          });
        }, 5000);
      });
    }
  }
}
