import * as _ from 'lodash';

import { Component, Input, Output, EventEmitter } from '@angular/core';
import { FileNode } from '../bridge.service';

@Component({
  selector: 'app-path-indicator',
  templateUrl: './path-indicator.component.html',
  styleUrls: ['./path-indicator.component.scss']
})
export class PathIndicatorComponent {
  currentPath = '/';
  paths: FileNode[] = [];

  @Input()
  get path() {
    return this.currentPath;
  }

  @Output()
  pathChange = new EventEmitter();

  set path(p: string) {
    this.currentPath = p;
    this.pathChange.emit(p);

    const nodes = p.split('/');
    const paths = _.map(_.range(0, nodes.length), (n: number) => _.slice(nodes, 0, n + 1).join('/'));
    this.paths = _.map(paths, (path: string) => ({
      children: [],
      name: _.last(path.split('/')),
      path: path,
      type: 'inode/directory'
    }));

    this.paths[0] = {
      children: [],
      name: 'Store',
      type: 'inode/directory',
      path: '/'
    };

    this.paths = _.filter(this.paths, (node: FileNode) => node.name !== '');
  }

  setPath(path: string) {
    this.path = path;
  }
}
