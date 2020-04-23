import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';

import { TranslatePipe } from './translation/translation.pipe';
import { TRANSLATION_PROVIDERS } from './translation/translation';
import { TranslateService } from './translation/translation.service';
import { FormsModule } from '@angular/forms';
import { HotkeyModule } from 'angular2-hotkeys';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { MatSnackBarModule } from '@angular/material/snack-bar';
import { MatToolbarModule } from '@angular/material/toolbar';

@NgModule({
  declarations: [
    AppComponent,
    TranslatePipe
  ],
  imports: [
    BrowserModule,
    BrowserAnimationsModule,
    FormsModule,
    MatButtonModule,
    MatCardModule,
    MatFormFieldModule,
    MatInputModule,
    MatSnackBarModule,
    MatToolbarModule,
    HotkeyModule.forRoot()
  ],
  providers: [
    TRANSLATION_PROVIDERS,
    TranslateService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
