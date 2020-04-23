import { Component, Inject } from '@angular/core';
import { MatDialogRef, MAT_DIALOG_DATA } from '@angular/material/dialog';

export interface InputDialogData {
  title: string;
  value: any;
  type: string;
}

@Component({
  selector: 'app-input-dialog',
  templateUrl: './input-dialog.component.html',
  styleUrls: ['./input-dialog.component.scss']
})
export class InputDialogComponent {
  title: string;
  value: any;
  type = 'text';

  constructor(
    private dialogRef: MatDialogRef<InputDialogComponent>,
    @Inject(MAT_DIALOG_DATA) public data: InputDialogData) {
    this.title = data.title;
    this.value = data.value;
    this.type = data.type;
  }

  ok() {
    this.dialogRef.close(this.value);
  }

  cancel() {
    this.dialogRef.close(null);
  }
}
