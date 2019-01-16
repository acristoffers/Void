import { Component, HostBinding } from '@angular/core';
import { BehaviorSubject } from 'rxjs';
import { Hotkey, HotkeysService } from 'angular2-hotkeys';
import { DomSanitizer, SafeUrl } from '@angular/platform-browser';

import * as _ from 'lodash';

@Component({
  selector: 'app-image-viewer',
  templateUrl: './image-viewer.component.html',
  styleUrls: ['./image-viewer.component.scss']
})
export class ImageViewerComponent {
  static images: BehaviorSubject<string[]> = new BehaviorSubject([]);
  static setCurrent: BehaviorSubject<string> = new BehaviorSubject('');
  static show: BehaviorSubject<boolean> = new BehaviorSubject(false);

  _images: string[] = [];
  @HostBinding('class.show') _show = false;
  _cursor = 0;

  constructor(
    private hotkeys: HotkeysService,
    private sanitizer: DomSanitizer
  ) {
    ImageViewerComponent.images.subscribe(images => this._images = images);
    ImageViewerComponent.setCurrent.subscribe(path => this._cursor = _.findIndex(this._images, i => i === path));
    ImageViewerComponent.show.subscribe(show => {
      this._show = show;
      this._cursor = 0;
    });

    this.hotkeys.add(new Hotkey('left', (event: KeyboardEvent): boolean => {
      this._cursor = _.sortBy([0, this._cursor - 1, this._images.length - 1])[1];
      return false;
    }));

    this.hotkeys.add(new Hotkey('right', (event: KeyboardEvent): boolean => {
      this._cursor = _.sortBy([0, this._cursor + 1, this._images.length - 1])[1];
      return false;
    }));

    this.hotkeys.add(new Hotkey('esc', (event: KeyboardEvent): boolean => {
      this._show = false;
      return false;
    }));
  }

  urlForCurrent(): SafeUrl {
    const url = `decrypt://${this._images[this._cursor]}`;
    return this.sanitizer.bypassSecurityTrustUrl(url);
  }
}
