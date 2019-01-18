import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { TranslatePipe } from './translation/translation.pipe';
import { TRANSLATION_PROVIDERS } from './translation/translation';
import { TranslateService } from './translation/translation.service';
import { ReactiveFormsModule, FormsModule } from '@angular/forms';
import { ToolbarComponent } from './toolbar/toolbar.component';
import { SidenavComponent } from './sidenav/sidenav.component';
import { HttpClientModule } from '@angular/common/http';
import { HotkeyModule } from 'angular2-hotkeys';

import {
  MatButtonModule,
  MatCardModule,
  MatFormFieldModule,
  MatInputModule,
  MatSnackBarModule,
  MatToolbarModule,
  MatSidenavModule,
  MatIconModule,
  MatMenuModule,
  MatButtonToggleModule,
  MatChipsModule,
  MatDialogModule,
  MatListModule
} from '@angular/material';

import { PathIndicatorComponent } from './path-indicator/path-indicator.component';
import { FileToolbarComponent } from './file-toolbar/file-toolbar.component';
import { FileGridComponent } from './file-grid/file-grid.component';
import { FileInfoComponent } from './file-info/file-info.component';
import { FileNodeTreeItemComponent } from './file-node-tree-item/file-node-tree-item.component';
import { InputDialogComponent } from './input-dialog/input-dialog.component';
import { HotkeysCheatsheetComponent } from './hotkeys-cheatsheet/hotkeys-cheatsheet.component';
import { ImageViewerComponent } from './image-viewer/image-viewer.component';
import { StatusListComponent } from './status-list/status-list.component';

@NgModule({
  declarations: [
    AppComponent,
    TranslatePipe,
    ToolbarComponent,
    SidenavComponent,
    PathIndicatorComponent,
    FileToolbarComponent,
    FileGridComponent,
    FileInfoComponent,
    FileNodeTreeItemComponent,
    InputDialogComponent,
    HotkeysCheatsheetComponent,
    ImageViewerComponent,
    StatusListComponent
  ],
  imports: [
    BrowserModule,
    BrowserAnimationsModule,
    FormsModule,
    ReactiveFormsModule,
    HttpClientModule,
    MatButtonModule,
    MatButtonToggleModule,
    MatCardModule,
    MatFormFieldModule,
    MatInputModule,
    MatSnackBarModule,
    MatToolbarModule,
    MatSidenavModule,
    MatIconModule,
    MatMenuModule,
    MatChipsModule,
    MatDialogModule,
    MatListModule,
    HotkeyModule.forRoot()
  ],
  providers: [
    TRANSLATION_PROVIDERS,
    TranslateService
  ],
  entryComponents: [InputDialogComponent],
  bootstrap: [AppComponent]
})
export class AppModule { }
