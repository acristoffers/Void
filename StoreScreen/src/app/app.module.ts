import { HttpClientModule } from '@angular/common/http';
import { NgModule } from '@angular/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { HotkeyModule } from 'angular2-hotkeys';
import { AppComponent } from './app.component';
import { FileGridComponent } from './file-grid/file-grid.component';
import { FileInfoComponent } from './file-info/file-info.component';
import { FileNodeTreeItemComponent } from './file-node-tree-item/file-node-tree-item.component';
import { FileToolbarComponent } from './file-toolbar/file-toolbar.component';
import { HotkeysCheatsheetComponent } from './hotkeys-cheatsheet/hotkeys-cheatsheet.component';
import { ImageViewerComponent } from './image-viewer/image-viewer.component';
import { InputDialogComponent } from './input-dialog/input-dialog.component';
import { PathIndicatorComponent } from './path-indicator/path-indicator.component';
import { SidenavComponent } from './sidenav/sidenav.component';
import { StatusListComponent } from './status-list/status-list.component';
import { TextEditorComponent } from './text-editor/text-editor.component';
import { ToolbarComponent } from './toolbar/toolbar.component';
import { TRANSLATION_PROVIDERS } from './translation/translation';
import { TranslatePipe } from './translation/translation.pipe';
import { TranslateService } from './translation/translation.service';
import { AceEditorModule } from 'ng2-ace-editor';

import {
  MatButtonModule,
  MatButtonToggleModule,
  MatCardModule,
  MatChipsModule,
  MatDialogModule,
  MatFormFieldModule,
  MatIconModule,
  MatInputModule,
  MatListModule,
  MatMenuModule,
  MatSidenavModule,
  MatSnackBarModule,
  MatToolbarModule,
  MatSelectModule,
  MatDividerModule,
  MatRadioModule
} from '@angular/material';
import { SearchDialogComponent } from './search-dialog/search-dialog.component';

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
    StatusListComponent,
    TextEditorComponent,
    SearchDialogComponent
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
    MatSelectModule,
    MatDividerModule,
    MatRadioModule,
    HotkeyModule.forRoot(),
    AceEditorModule
  ],
  providers: [
    TRANSLATION_PROVIDERS,
    TranslateService
  ],
  entryComponents: [
    InputDialogComponent,
    TextEditorComponent,
    SearchDialogComponent
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
