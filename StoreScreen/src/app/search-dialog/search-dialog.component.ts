import * as _ from 'lodash';
import { Component, OnDestroy, OnInit, NgZone } from '@angular/core';
import { Subscription, zip } from 'rxjs';
import { BridgeService, FileNode } from '../bridge.service';
import { MatDialogRef } from '@angular/material';

interface SearchItem {
  path: string;
  mime: string;
  tags: string[];
  comments: string;
}

@Component({
  selector: 'app-search-dialog',
  templateUrl: './search-dialog.component.html',
  styleUrls: ['./search-dialog.component.scss']
})
export class SearchDialogComponent implements OnInit, OnDestroy {
  private treeSubscription: Subscription = null;
  searchTerm = '';
  entries: SearchItem[] = [];
  filteredEntries: SearchItem[] = [];

  constructor(
    private dialogRef: MatDialogRef<SearchDialogComponent>,
    private bridge: BridgeService,
    private zone: NgZone) {
  }

  ngOnInit() {
    this.treeSubscription = this.bridge.fileTreeObservable().subscribe(tree => {
      function flatten(xs: string[], node: FileNode): string[] {
        return _.concat(xs, node.path, _.flatMap(node.children, c => flatten(xs, c)));
      }

      this.entries = [];
      flatten([], tree).forEach(p => {
        const mimeobs = this.bridge.fileMetadata(p, 'mimetype');
        const tagsobs = this.bridge.fileMetadata(p, 'tags');
        const commentsobs = this.bridge.fileMetadata(p, 'comments');

        zip(mimeobs, tagsobs, commentsobs).subscribe(v => {
          const [mime, tags, comments] = v;
          const entry: SearchItem = {
            path: p,
            mime: mime || 'inode/directory',
            tags: JSON.parse(tags || '[]'),
            comments: comments || ''
          };

          setTimeout(() => {
            this.entries = _.concat(this.entries, entry);
            this.filteredEntries = this.entries;
          }, 1);
        });
      });
    });
  }

  ngOnDestroy() {
    if (this.treeSubscription) {
      this.treeSubscription.unsubscribe();
    }
  }

  search() {
    const term = this.searchTerm.toLowerCase();
    this.filteredEntries = _.filter(this.entries, e => {
      const path = _.includes(e.path.toLowerCase(), term);
      const tags = _.filter(e.tags, t => _.includes(t.toLowerCase(), term)).length !== 0;
      const coms = _.includes(e.comments.toLowerCase(), term);
      return path || tags || coms;
    });
  }

  open(item: SearchItem) {
    // const path = item.mime === 'inode/directory' ? item.path : _.slice(item.path.split('/'), 0, -1).join('/');
    this.dialogRef.close(item.path);
  }
}
