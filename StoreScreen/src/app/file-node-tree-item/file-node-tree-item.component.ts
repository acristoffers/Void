import { Component, Input, Output, EventEmitter } from '@angular/core';
import { FileNode } from '../bridge.service';

import * as _ from 'lodash';
import { MatMenuTrigger } from '@angular/material';

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
}
