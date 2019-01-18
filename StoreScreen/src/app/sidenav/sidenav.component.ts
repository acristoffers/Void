import * as _ from 'lodash';

import { Component, Input, Output, EventEmitter, NgZone } from '@angular/core';
import { FileNode, BridgeService } from '../bridge.service';

@Component({
  selector: 'app-sidenav',
  templateUrl: './sidenav.component.html',
  styleUrls: ['./sidenav.component.scss']
})
export class SidenavComponent {
  rootNode: FileNode = {
    children: [],
    name: 'Store',
    path: '/',
    type: 'inode/directory'
  };

  currentPath = '/';

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
    private bridge: BridgeService,
    private zone: NgZone
  ) {
    this.bridge.fileTreeObservable().subscribe(tree => {
      this.zone.run(() => {
        this.rootNode = this.removeFiles(_.cloneDeep(tree));
      });
    });
  }

  private removeFiles(node: FileNode): FileNode {
    node.children = _.map(_.filter(node.children, (c: FileNode) => c.type === 'inode/directory'), (n: FileNode) => this.removeFiles(n));
    return node;
  }
}
