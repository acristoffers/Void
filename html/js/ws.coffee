###
 * Copyright (c) 2015 Álan Crístoffer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
###

$ ->
    wb = null
    action = ''

    $('#load').click ->
        wb.getExisting (res) ->
            if res
                $('#actions').css('display', 'none')
                $('#info').css('display', 'block')
                action = 'load'

    $('#create').click ->
        wb.getNew (res) ->
            if res
                $('#actions').css('display', 'none')
                $('#info').css('display', 'block')
                action = 'create'

    $('#password').on 'keydown', (e) ->
        if e.which == 13
            e.preventDefault();
            if action == 'load'
                wb.open $('#password').val(), (r) ->
                    process_error r
            else if action == 'create'
                wb.create $('#password').val(), (r) ->
                    process_error r
            $(this).val('')

    process_error = (error) ->
        switch error
            when 'Success' then wb.close()
            when 'DoesntExistAndCreationIsNotPermitted' then alert window.tr "DoesntExistAndCreationIsNotPermitted";
            when 'CantCreateCryptoObject' then alert window.tr "CantCreateCryptoObject";
            when 'CantOpenStoreFile' then alert window.tr "CantOpenStoreFile";
            when 'CantOpenFile' then alert window.tr "CantOpenFile";
            when 'CantWriteToFile' then alert window.tr "CantWriteToFile";
            when 'FileTooLarge' then alert window.tr "FileTooLarge";
            when 'NoSuchFile' then alert window.tr "NoSuchFile";
            when 'PartCorrupted' then alert window.tr "PartCorrupted";
            when 'WrongCheckSum' then alert window.tr "WrongCheckSum";
            when 'FileAlreadyExists' then alert window.tr "FileAlreadyExists";

    $('#cancel').click ->
        $('#actions').css('display', 'block')
        $('#info').css('display', 'none')

    new QWebChannel qt.webChannelTransport, (channel) ->
        wb = channel.objects.welcome_bridge;

        $(window.trs).each ->
            locale = this
            $('#' + locale).click ->
                window.locale = locale
                window.update_translation()
                wb.setLang locale

        wb.lang (r) ->
            window.locale = r
            window.update_translation()
