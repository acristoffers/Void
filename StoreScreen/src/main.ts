import './polyfills';

import { enableProdMode } from '@angular/core';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { AppModule } from './app/app.module';
import { environment } from './environments/environment';

declare let QWebChannel: any;
declare let qt: any;

const _qwc = new QWebChannel(qt.webChannelTransport, (channel: any) => {
  (window as any).bridge = channel.objects.store_bridge;
  (window as any).store = channel.objects.store;

  if (environment.production) {
    enableProdMode();
  }

  platformBrowserDynamic().bootstrapModule(AppModule).catch(console.error);
});
