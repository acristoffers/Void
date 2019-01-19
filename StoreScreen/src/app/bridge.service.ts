import { Injectable } from '@angular/core';
import * as _ from 'lodash';
import { BehaviorSubject, bindCallback, forkJoin, Observable, of } from 'rxjs';
import { debounceTime, flatMap, map } from 'rxjs/operators';
import * as sf from 'sanitize-filename';
import { StatusItem } from './status-list/status-list.component';
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
  decryptFile(storePath: string, callback: (path: string) => void): void;
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
  static keyPressedSubject: BehaviorSubject<string> = null;
  static statusChange: BehaviorSubject<StatusItem> = null;
  treeUpdateDebounce = new BehaviorSubject<void>(null);

  constructor(
    private translate: TranslateService
  ) {
    if (BridgeService.fileTreeSubject == null) {
      BridgeService.fileTreeSubject = new BehaviorSubject(this.rootNode());
    }

    if (BridgeService.keyPressedSubject == null) {
      BridgeService.keyPressedSubject = new BehaviorSubject(null);
    }

    if (BridgeService.statusChange == null) {
      BridgeService.statusChange = new BehaviorSubject(null);
    }

    this.treeUpdateDebounce.pipe(debounceTime(1000)).subscribe(() => {
      this.generateTree().subscribe();
    });

    bridge.startAddFile.connect((fsPath: string, __: string) => {
      BridgeService.statusChange.next({
        type: 'addStart',
        path: fsPath
      });
    });

    bridge.endAddFile.connect((fsPath: string, __: string) => {
      this.treeUpdateDebounce.next(null);
      BridgeService.statusChange.next({
        type: 'addEnd',
        path: fsPath
      });
    });

    bridge.startDecryptFile.connect((fsPath: string) => {
      BridgeService.statusChange.next({
        type: 'decryptStart',
        path: fsPath
      });
    });

    bridge.endDecryptFile.connect((fsPath: string) => {
      BridgeService.statusChange.next({
        type: 'decryptEnd',
        path: fsPath
      });
    });

    this.generateTree().subscribe();
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
    return addFileFromData(path, 'placeholder').pipe(flatMap(() => this.generateTree()));
  }

  createDir(path: string): Observable<void> {
    const makePath = bindCallback(store.makePath);
    return makePath(path).pipe(flatMap(() => this.generateTree()));
  }

  addFile(path: string) {
    const getFile = bindCallback(bridge.getFile);
    getFile().subscribe(fs => {
      const addFile = bindCallback(store.addFile);

      fs.forEach(f => {
        const fileName = _.last(f.split('/'));
        BridgeService.statusChange.next({
          type: 'addStart',
          path: f
        });

        addFile(f, this.appendPath(path, fileName)).subscribe(() => {
          this.generateTree().subscribe();
          BridgeService.statusChange.next({
            type: 'addEnd',
            path: f
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

        // next is quite slow, so it avoids freezing the screen.
        function g(xs: string[]) {
          if (_.isEmpty(xs)) {
            return;
          }

          BridgeService.statusChange.next({
            type: 'addStart',
            path: _.head(xs)
          });

          setTimeout(() => g(_.tail(xs)), 1);
        }

        g(fs);
      });
    });
  }

  remove(path: string, ask: boolean = true): Observable<void> {
    const msg = this.translate.instant('Are you sure that you want to delete this item?');
    if (!ask || confirm(msg)) {
      const remove = bindCallback(store.remove);
      return remove(path).pipe(flatMap(() => this.generateTree()));
    } else {
      return of();
    }
  }

  move(from: string, to: string): Observable<void> {
    const move = bindCallback(store.move);
    return move(from, to).pipe(flatMap(() => this.generateTree()));
  }

  decrypt(path: string[], currentPath: string) {
    const decrypt = bindCallback(bridge.decrypt);
    return decrypt(path, currentPath);
  }

  decryptFile(path: string): Observable<string> {
    const decryptFile = bindCallback(store.decryptFile);
    return decryptFile(path);
  }

  saveFile(path: string, data: string) {
    const addFile = bindCallback(store.addFileFromData);
    return addFile(path, data).pipe(flatMap(() => this.generateTree()));
  }

  sanitizeFileName(name: string): string {
    return sf(name);
  }

  appendPath(path: string, name: string): string {
    return `${path}/${this.sanitizeFileName(name)}`.replace('//', '/');
  }

  playVideo(node: FileNode) {
    if (node.type.startsWith('video')) {
      const playVideo = bindCallback(bridge.playVideo);
      return playVideo(node.path);
    }
  }

  setting(key: string, value?: any): Observable<any | void> {
    if (value) {
      const settings = bindCallback(bridge.saveSetting);
      return settings(key, value);
    } else {
      const settings = bindCallback(bridge.setting);
      return settings(key);
    }
  }

  private rootNode(): FileNode {
    return {
      children: [],
      name: 'Store',
      type: 'inode/directory',
      path: '/'
    };
  }

  private generateTree(): Observable<void> {
    const listAllEntries = bindCallback(store.listAllEntries);
    return listAllEntries().pipe(flatMap(fs => {
      const tree: FileNode = this.rootNode();

      // this is the case of an empty store. The sole element is '/'.
      if (fs.length === 1) {
        BridgeService.fileTreeSubject.next(tree);
        return of();
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

      return forkJoin(...observables).pipe(map(() => {
        BridgeService.fileTreeSubject.next(tree);
      }));
    }));
  }
}
