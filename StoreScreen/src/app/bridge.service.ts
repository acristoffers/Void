import { Injectable, NgZone } from '@angular/core';
import { bindCallback, BehaviorSubject } from 'rxjs';
import { Observable, forkJoin, of } from 'rxjs';
import { map, debounceTime } from 'rxjs/operators';

import * as sf from 'sanitize-filename';
import * as _ from 'lodash';
import { MatSnackBar } from '@angular/material';
import { TranslateService } from './translation';

declare class Bridge {
  routeSignalSignal: any;
  startAddFile: any;
  endAddFile: any;
  startDecryptFile: any;
  endDecryptFile: any;

  setLang(lang: string): void;
  lang(callback: (lang: string) => void): void;
  asyncAddFile(fsPath: string, storePath: string): void;
  decrypt(paths: string[], currentPath: string): void;
  getFile(callback: (files: string[]) => void): void;
  getFolder(callback: (folder: string) => void): void;
  listFilesInFolder(folder: string, callback: (files: string[]) => void): void;
  playVideo(path: string): void;
  saveSetting(key: string, value: string): void;
  setting(key: string, callback: (value: string) => void): void;
}

declare class Store {
  addFileFromData(storePath: string, data: string, callback: () => void): void;
  addFile(filePath: string, storePath: string, callback: () => void): void;
  decryptFile(storePath: string, path: string, callback: () => void): void;
  move(oldPath: string, newPath: string, callback: () => void): void;
  remove(path: string, callback: () => void): void;
  makePath(path: string, callback: () => void): void;
  listAllDirectories(callback: (ds: string[]) => void): void;
  listAllEntries(callback: (es: string[]) => void): void;
  listAllFiles(callback: (fs: string[]) => void): void;
  listSubdirectories(path: string, callback: (ds: string[]) => void): void;
  listFiles(path: string, callback: (fs: string[]) => void): void;
  listEntries(path: string, callback: (es: string[]) => void): void;
  searchStartsWith(filter: string, type: number, callback: (es: string[]) => void): void;
  searchEndsWith(filter: string, type: number, callback: (es: string[]) => void): void;
  searchContains(filter: string, type: number, callback: (es: string[]) => void): void;
  searchRegex(filter: string, type: number, callback: (es: string[]) => void): void;
  fileMetadata(path: string, key: string, callback: (md: string) => void): void;
  setFileMetadata(path: string, key: string, data: string, callback: () => void): void;
  fileSize(path: string, callback: (size: number) => void): void;
}

declare const bridge: Bridge;
declare const store: Store;

export class FileNode {
  children: FileNode[];
  name: string;
  type: string;
  path: string;
}

@Injectable({
  providedIn: 'root'
})
export class BridgeService {
  static fileTreeSubject: BehaviorSubject<FileNode> = null;
  treeUpdateDebounce = new BehaviorSubject<void>(null);

  constructor(
    private toast: MatSnackBar,
    private translate: TranslateService,
    private zone: NgZone
  ) {
    if (BridgeService.fileTreeSubject == null) {
      BridgeService.fileTreeSubject = new BehaviorSubject(this.rootNode());
    }

    this.treeUpdateDebounce.pipe(debounceTime(1000)).subscribe(() => {
      this.generateTree();
    });

    bridge.startAddFile.connect((fsPath, __) => {
      this.zone.run(() => {
        const msg1 = this.translate.instant('Adding %s').replace('%s', fsPath);
        this.toast.open(msg1, null, { duration: 2000 });
      });
    });

    bridge.endAddFile.connect((fsPath: string, __: string) => {
      this.treeUpdateDebounce.next(null);

      this.zone.run(() => {
        const msg1 = this.translate.instant('Added %s').replace('%s', fsPath);
        this.toast.open(msg1, null, { duration: 2000 });
      });
    });

    bridge.startDecryptFile.connect((fsPath) => {
      this.zone.run(() => {
        const msg1 = this.translate.instant('Decrypting %s').replace('%s', fsPath);
        this.toast.open(msg1, null, { duration: 2000 });
      });
    });

    bridge.endDecryptFile.connect((fsPath) => {
      this.zone.run(() => {
        const msg1 = this.translate.instant('Decrypted %s').replace('%s', fsPath);
        this.toast.open(msg1, null, { duration: 2000 });
      });
    });

    this.generateTree();
  }

  fileTreeObservable(): Observable<FileNode> {
    return BridgeService.fileTreeSubject.asObservable();
  }

  setLang(lang: string) {
    bridge.setLang(lang);
  }

  lang(): Observable<string> {
    const lang = bindCallback(bridge.lang);
    return lang();
  }

  fileMetadata(path: string, key: string): Observable<string> {
    const fileMetadata = bindCallback(store.fileMetadata);
    return fileMetadata(path, key);
  }

  createFile(path: string): Observable<void> {
    const addFileFromData = bindCallback(store.addFileFromData);
    return addFileFromData(path, 'placeholder').pipe(map(() => this.generateTree()));
  }

  createDir(path: string): Observable<void> {
    const makePath = bindCallback(store.makePath);
    return makePath(path).pipe(map(() => this.generateTree()));
  }

  addFile(path: string) {
    const getFile = bindCallback(bridge.getFile);
    getFile().subscribe(fs => {
      const addFile = bindCallback(store.addFile);

      fs.forEach(f => {
        const fileName = _.last(f.split('/'));
        this.zone.run(() => {
          const msg1 = this.translate.instant('Adding %s').replace('%s', f);
          this.toast.open(msg1, null, { duration: 2000 });
        });

        addFile(f, this.appendPath(path, fileName)).subscribe(() => {
          this.generateTree();

          this.zone.run(() => {
            const msg2 = this.translate.instant('Added %s').replace('%s', f);
            this.toast.open(msg2, null, { duration: 2000 });
          });
        });
      });
    });
  }

  addFolder(path: string) {
    const getFolder = bindCallback(bridge.getFolder);
    getFolder().subscribe(folder => {
      if (folder.length === 0) {
        return;
      }

      const folderPath = _.slice(folder.split('/'), 0, -1).join('/');
      const listFilesInFolder = bindCallback(bridge.listFilesInFolder);
      listFilesInFolder(folder).subscribe(fs => {
        fs.forEach(f => {
          const storePath = f.replace(folderPath, path).replace('//', '/');
          bridge.asyncAddFile(f, storePath);
        });
      });
    });
  }

  remove(path: string, ask: boolean = true): Observable<void> {
    const msg = this.translate.instant('Are you sure that you want to delete this item?');
    if (!ask || confirm(msg)) {
      const remove = bindCallback(store.remove);
      return remove(path).pipe(map(() => this.generateTree()));
    } else {
      return of();
    }
  }

  move(from: string, to: string): Observable<void> {
    const move = bindCallback(store.move);
    return move(from, to).pipe(map(() => this.generateTree()));
  }

  decrypt(path: string[], currentPath: string) {
    const decrypt = bindCallback(bridge.decrypt);
    return decrypt(path, currentPath);
  }

  sanitizeFileName(name: string): string {
    return sf(name);
  }

  appendPath(path: string, name: string): string {
    return `${path}/${this.sanitizeFileName(name)}`.replace('//', '/');
  }

  private rootNode(): FileNode {
    return {
      children: [],
      name: 'Store',
      type: 'inode/directory',
      path: '/'
    };
  }

  private generateTree() {
    const listAllEntries = bindCallback(store.listAllEntries);
    listAllEntries().subscribe(fs => {
      const tree: FileNode = this.rootNode();

      // this is the case of an empty store. The sole element is '/'.
      if (fs.length === 1) {
        BridgeService.fileTreeSubject.next(tree);
        return;
      }

      const observables: Observable<void>[] = [];

      _.forEach(fs, (f: string) => {
        const nodes = _.filter(f.split('/'), n => n !== '');

        if (nodes.length === 0) {
          return;
        }

        let dir = tree;
        _.forEach(_.slice(nodes, 0, nodes.length - 1), (d: string) => {
          const has = _.filter(dir.children, (c: FileNode) => c.name === d);

          if (!_.isEmpty(has)) {
            dir = _.first(has);
          } else {
            const node = {
              children: [],
              name: d,
              type: 'inode/directory',
              path: `${dir.path}/${d}`.replace('//', '/')
            };

            dir.children.push(node);
            dir = node;
          }
        });

        const self = this;
        (function (path: string, directory: FileNode) {
          const obs = self.fileMetadata(path, 'mimetype').pipe(map((md: string) => {
            if (md == null && _.filter(directory.children, c => c.path === path).length !== 0) {
              return;
            }

            const file = {
              children: [],
              name: _.last(path.split('/')),
              type: md || 'inode/directory',
              path: path
            };

            directory.children.push(file);
          }));

          observables.push(obs);
        })(f, dir);
      });

      forkJoin(...observables).subscribe(() => {
        BridgeService.fileTreeSubject.next(tree);
      });
    });
  }
}
