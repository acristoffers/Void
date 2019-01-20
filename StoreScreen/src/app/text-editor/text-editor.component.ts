import { Component, Inject, NgZone } from '@angular/core';
import { MatDialogRef, MAT_DIALOG_DATA } from '@angular/material';
import * as _ from 'lodash';
import { flatMap } from 'rxjs/operators';
import { BridgeService } from '../bridge.service';

export interface TextEditorData {
  filePath: string;
  mimetype: string;
}

@Component({
  selector: 'app-text-editor',
  templateUrl: './text-editor.component.html',
  styleUrls: ['./text-editor.component.scss']
})
export class TextEditorComponent {
  static Languages = ['abap', 'handlebars', 'praat', 'abc', 'haskell', 'prolog',
    'actionscript', 'haskell_cabal', 'properties', 'ada', 'haxe', 'protobuf',
    'apache_conf', 'hjson', 'puppet', 'apex', 'html', 'python',
    'applescript', 'html_elixir', 'r', 'asciidoc', 'html_ruby', 'razor',
    'asl', 'ini', 'rdoc', 'assembly_x86', 'io', 'red', 'autohotkey', 'jack',
    'redshift', 'batchfile', 'jade', 'rhtml', 'bro', 'java', 'rst',
    'c9search', 'javascript', 'ruby', 'c_cpp', 'json', 'rust', 'cirru',
    'jsoniq', 'sass', 'clojure', 'jsp', 'scad', 'cobol', 'jssm', 'scala',
    'coffee', 'jsx', 'scheme', 'coldfusion', 'julia', 'scss', 'csharp',
    'kotlin', 'sh', 'csound_document', 'latex', 'sjs', 'csound_orchestra',
    'less', 'slim', 'csound_score', 'liquid', 'smarty', 'csp', 'lisp',
    'snippets', 'css', 'livescript', 'soy_template', 'curly', 'logiql',
    'space', 'd', 'logtalk', 'sparql', 'dart', 'lsl', 'sql', 'diff', 'lua',
    'sqlserver', 'django', 'luapage', 'stylus', 'dockerfile', 'lucene',
    'svg', 'dot', 'makefile', 'swift', 'drools', 'markdown', 'tcl',
    'edifact', 'mask', 'terraform', 'eiffel', 'matlab', 'tex', 'ejs', 'maze',
    'text', 'elixir', 'mel', 'textile', 'elm', 'mixal', 'toml', 'erlang',
    'mushcode', 'tsx', 'forth', 'mysql', 'turtle', 'fortran', 'nix', 'twig',
    'fsharp', 'nsis', 'typescript', 'fsl', 'objectivec', 'vala', 'ftl',
    'ocaml', 'vbscript', 'gcode', 'pascal', 'velocity', 'gherkin', 'perl',
    'verilog', 'gitignore', 'perl6', 'vhdl', 'glsl', 'pgsql', 'visualforce',
    'gobstones', 'php', 'wollok', 'golang', 'php_laravel_blade', 'xml',
    'graphqlschema', 'pig', 'xquery', 'groovy', 'plain_text', 'yaml', 'haml',
    'powershell'];

  static Themes = ['ambiance', 'chaos', 'chrome', 'clouds', 'clouds_midnight',
    'cobalt', 'crimson_editor', 'dawn', 'dreamweaver', 'eclipse', 'github',
    'idle_fingers', 'iplastic', 'katzenmilch', 'kr_theme', 'kuroir',
    'merbivore', 'merbivore_soft', 'mono_industrial', 'monokai',
    'pastel_on_dark', 'solarized_dark', 'solarized_light', 'sqlserver',
    'terminal', 'textmate', 'tomorrow', 'tomorrow_night',
    'tomorrow_night_blue', 'tomorrow_night_bright', 'tomorrow_night_eighties',
    'twilight', 'vibrant_ink', 'xcode'];

  path: string = null;
  mimetype: string = null;
  fileContent = '';
  language = 'python';
  _theme = 'xcode';
  languages: string[] = [];
  themes: string[] = [];

  set theme(theme: string) {
    this._theme = theme;
    this.bridge.setting('editor-theme', theme).subscribe();
  }

  get theme(): string {
    return this._theme;
  }

  constructor(
    private dialogRef: MatDialogRef<TextEditorComponent>,
    @Inject(MAT_DIALOG_DATA) public data: TextEditorData,
    private bridge: BridgeService,
    private zone: NgZone
  ) {
    this.dialogRef.disableClose = true;

    this.languages = _.sortBy(TextEditorComponent.Languages);
    this.themes = _.sortBy(TextEditorComponent.Themes);

    this.path = data.filePath;
    this.mimetype = data.mimetype;

    this.bridge.setting('editor-theme').subscribe((theme: string) => {
      this.theme = theme || 'xcode';
    });

    this.language = TextEditorComponent.modeForMime(this.mimetype);
    this.language = _.includes(this.languages, this.language) ? this.language : 'text';
    this.bridge.decryptFile(this.path).subscribe(d => this.fileContent = d);
  }

  static modeForMime(mime: string): string {
    if (_.includes(['text/x-c', 'text/x-csrc', 'text/x-cpp', 'text/x-cppsrc',
      'text/x-cxx', 'text/x-cxxsrc', 'text/x-c++', 'text/x-c++src',
      'text/x-chdr'], mime)) {
      return 'c_cpp';
    } else if (_.includes(['text/x-r', 'text/x-rsrc'], mime)) {
      return 'r';
    } else if (_.includes(['text/x-d', 'text/x-dsrc'], mime)) {
      return 'd';
    }

    const normalize = (s: string) => s.toLowerCase()
      .replace('text/', '')
      .replace('application/', '') // otherwise IO matches
      .replace('audio/', '') // otherwise IO matches
      .replace(/[_\/\\\.]/g, '-');

    const languages = _.sortBy(_.filter(TextEditorComponent.Languages, l => !_.includes(['d', 'r', 'c_cpp'], l)));
    const directMatch = _.maxBy(_.filter(languages, l => _.includes(normalize(mime), normalize(l))), i => i.length);
    const partialMatch = _.first(_.filter(languages, l => _.some(normalize(l).split('-'), p => _.includes(normalize(mime), p))));
    return directMatch || partialMatch || 'text_plain';
  }

  save() {
    this.bridge
      .remove(this.path, false)
      .pipe(flatMap(() => {
        return this.bridge.saveFile(this.path, this.fileContent);
      }))
      .subscribe(() => {
        this.zone.run(() => this.dialogRef.close());
      });
  }

  cancel() {
    this.dialogRef.close();
  }
}
