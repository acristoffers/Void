import { Component, OnDestroy, OnInit } from '@angular/core';
import { MatDialogRef } from '@angular/material';
import * as _ from 'lodash';
import { Subscription, zip } from 'rxjs';
import { BridgeService, FileNode } from '../bridge.service';

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
  _searchType = 'all';
  entries: SearchItem[] = [];
  filteredEntries: SearchItem[] = [];

  get searchType(): string {
    return this._searchType;
  }

  set searchType(type: string) {
    this._searchType = type;
    this.search();
  }

  constructor(
    private dialogRef: MatDialogRef<SearchDialogComponent>,
    private bridge: BridgeService
  ) {
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
    const terms = this.searchTerm.toLowerCase().split(/[ ]+/);

    const matches = _.map(terms, term => {
      return _.filter(this.entries, e => {
        const path = _.includes(e.path.toLowerCase(), term);
        const tags = _.filter(e.tags, t => t.toLowerCase() === term).length !== 0;
        const coms = _.includes(e.comments.toLowerCase(), term);
        return path || tags || coms;
      });
    });

    const method = {
      all: _.intersectionBy,
      any: _.unionBy
    }[this.searchType];

    this.filteredEntries = method(...[...matches, 'path']);
  }

  open(item: SearchItem) {
    this.dialogRef.close(item.path);
  }
}
