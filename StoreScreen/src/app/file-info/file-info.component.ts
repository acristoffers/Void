import { COMMA, ENTER } from '@angular/cdk/keycodes';
import { Component, NgZone } from '@angular/core';
import { DomSanitizer, SafeUrl } from '@angular/platform-browser';
import * as _ from 'lodash';
import { filter } from 'rxjs/operators';
import { BridgeService } from '../bridge.service';
import { TextEditorComponent } from '../text-editor/text-editor.component';
import { MatChipInputEvent } from '@angular/material';

@Component({
  selector: 'app-file-info',
  templateUrl: './file-info.component.html',
  styleUrls: ['./file-info.component.scss']
})
export class FileInfoComponent {
  readonly separatorKeysCodes: number[] = [ENTER, COMMA];
  filePath = '';
  fileName = '';
  fileType = '';
  fileSize = '';
  fileComments = '';
  fileTags: string[] = [];

  constructor(
    private zone: NgZone,
    private sanitizer: DomSanitizer,
    private bridge: BridgeService
  ) {
    BridgeService.fileInfo.pipe(filter(f => f != null)).subscribe(file => {
      this.filePath = file.path;
      this.fileName = _.last(file.path.split('/'));
      this.fileType = file.type;

      this.bridge.fileSize(this.filePath).subscribe(size => {
        this.zone.run(() => {
          if (size === 0) {
            this.fileSize = '0 B';
          } else {
            this.fileSize = this.humanFileSize(size);
          }
        });
      });

      this.bridge.fileMetadata(this.filePath, 'tags').subscribe(tags => {
        this.zone.run(() => this.fileTags = JSON.parse(tags || '[]'));
      });

      this.bridge.fileMetadata(this.filePath, 'comments').subscribe(comments => {
        this.zone.run(() => this.fileComments = comments);
      });
    });
  }

  icon(): string {
    const mimetype = this.fileType;
    if (mimetype === 'inode/directory') {
      return 'folder';
    } else if (mimetype.startsWith('image')) {
      return 'photo';
    } else if (mimetype.startsWith('video')) {
      return 'movie';
    } else if (mimetype.startsWith('audio')) {
      return 'audiotrack';
    } else if (this.isText(mimetype)) {
      return 'edit';
    } else {
      return 'insert_drive_file';
    }
  }

  isText(mime: string): boolean {
    return mime.startsWith('text') || TextEditorComponent.modeForMime(mime) !== 'text_plain';
  }

  urlFor(): SafeUrl {
    const url = `thumb://${this.filePath}`;
    return this.sanitizer.bypassSecurityTrustUrl(url);
  }

  addTag(event: MatChipInputEvent): void {
    const input = event.input;
    const value = event.value;

    if ((value || '').trim()) {
      this.fileTags = _.concat(this.fileTags, value.trim());
      this.bridge.setFileMetadata(this.filePath, 'tags', JSON.stringify(this.fileTags)).subscribe();
    }

    if (input) {
      input.value = '';
    }
  }

  removeTag(tag: string): void {
    this.fileTags = _.filter(this.fileTags, t => t !== tag);
    this.bridge.setFileMetadata(this.filePath, 'tags', JSON.stringify(this.fileTags)).subscribe();
  }

  saveComments() {
    this.bridge.setFileMetadata(this.filePath, 'comments', this.fileComments).subscribe();
  }

  private humanFileSize(size: number): string {
    const i = Math.floor(Math.log(size) / Math.log(1024));
    return parseFloat((size / Math.pow(1024, i)).toFixed(2)) + ' ' + ['B', 'kB', 'MB', 'GB', 'TB'][i];
  }
}
