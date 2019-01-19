import * as _ from 'lodash';

import { Component, HostBinding, NgZone } from '@angular/core';
import { BridgeService, FileNode } from '../bridge.service';
import { filter } from 'rxjs/operators';

export interface StatusItem {
  type: string;
  path: string;
}

@Component({
  selector: 'app-status-list',
  templateUrl: './status-list.component.html',
  styleUrls: ['./status-list.component.scss']
})
export class StatusListComponent {
  items: StatusItem[] = [];

  @HostBinding('class.show') show = false;

  constructor(private zone: NgZone) {
    BridgeService.statusChange.pipe(filter(i => i != null && i.type.endsWith('Start'))).subscribe(item => {
      this.zone.run(() => {
        this.show = true;
        const samePath = _.filter(this.items, i => i.path === item.path);
        if (_.isEmpty(samePath)) {
          this.items = _.concat(item, this.items);
        } else {
          this.items = _.concat(item, _.filter(this.items, i => i.path !== item.path));
        }
      });
    });

    BridgeService.statusChange.pipe(filter(i => i != null && i.type.endsWith('End'))).subscribe(item => {
      this.zone.run(() => {
        this.items = _.filter(this.items, i => i.path !== item.path);
        this.show = this.items.length !== 0;
      });
    });
  }

  baseName(path: string): string {
    return _.last(path.split('/'));
  }

  iconFor(node: FileNode) {
    return {
      addStart: 'add',
      decryptStart: 'launch',
      removeStart: 'delete'
    }[node.type];
  }
}
