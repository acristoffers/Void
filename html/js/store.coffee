###
 * Copyright (c) 2015 Álan Crístoffer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the 'Software'), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
###

unless Array.prototype.last
    Array.prototype.last = ->
        return this[this.length - 1]

unless Array.prototype.first
    Array.prototype.first = ->
        return this[0]

sb = null
store = null
path = ''
path_tree_node = {}
editor = null

supported_languages = ['abap', 'abc', 'actionscript', 'ada', 'apache', 'applescript',
'asciidoc', 'assembler', 'assembly', 'autohotkey', 'batchfile', 'c9search', 'c_cpp',
'cirru', 'clojure', 'cobol', 'coffee', 'coldfusion', 'conf', 'csharp', 'css',
'curly', 'dart', 'diff', 'django', 'dockerfile', 'dot', 'eiffel', 'elixir', 'r', 'io',
'elixir', 'elm', 'erlang', 'forth', 'fortran', 'ftl', 'gcode', 'gherkin',
'gitignore', 'glsl', 'gobstones', 'golang', 'groovy', 'haml', 'handlebars',
'haskell', 'haxe', 'html', 'html', 'html', 'ini', 'jack', 'jade', 'java',
'javascript', 'julia', 'latex', 'lean', 'less', 'liquid', 'lisp', 'live',
'livescript', 'log', 'logiql', 'lsl', 'lua', 'luapage', 'lucene', 'makefile',
'markdown', 'mask', 'mate', 'matlab', 'mavens', 'maze', 'mel', 'mips',
'mipsassembler', 'modeon', 'modeoniq', 'modep', 'modex', 'mushcode', 'mysql',
'nix', 'nsis', 'objectivec', 'ocaml', 'pascal', 'perl', 'pgsql', 'php',
'plain', 'powershell', 'praat', 'prolog', 'properties', 'protobuf', 'python',
'razor', 'rdoc', 'rhtml', 'rst', 'ruby', 'rust', 'sass', 'scad',
'scala', 'scheme', 'script', 'scss', 'sh', 'smarty', 'snippets', 'soy', 'space',
'sql', 'sqlserver', 'stylus', 'svg', 'swift', 'swig', 'tcl', 'template', 'tex',
'text', 'textile', 'toml', 'twig', 'typescript', 'vala', 'vbscript', 'velocity',
'verilog', 'vhdl', 'wollok', 'x86', 'xml', 'xquery', 'yaml']

ace_themes = ['ambiance', 'chaos', 'chrome', 'clouds', 'clouds_midnight', 'cobalt',
'crimson_editor', 'dawn', 'dreamweaver', 'eclipse', 'github', 'idle_fingers', 'iplastic',
'katzenmilch', 'kr_theme', 'kuroir', 'merbivore', 'merbivore_soft', 'mono_industrial',
'monokai', 'pastel_on_dark', 'solarized_dark', 'solarized_light', 'sqlserver', 'terminal',
'textmate', 'tomorrow', 'tomorrow_night', 'tomorrow_night_blue', 'tomorrow_night_bright',
'tomorrow_night_eighties', 'twilight', 'vibrant_ink', 'xcode']

time = ->
    new Date().getTime()

update_views = ->
    set_path path
    update_tree()

is_text = (mimetype) ->
    m = supported_languages.filter (e) -> e.length > 2
    m = m.map (lang) ->
        { lang: lang, percent: if mimetype.includes lang then lang.length / mimetype.length else 0 }

    f = (acc, e) ->
        if e.percent > acc.percent then e else acc

    r = m.reduce f
    if r.percent > 0 then r.lang else null

set_path = (new_path) ->
    deselect $('.entry')
    path = new_path || ''
    path_parts = path.split '/'
    path_parts = [''] if path_parts.last() == ''
    composite_path = ''
    html = ''

    for part in path_parts
        composite_path += '/' + part
        composite_path = composite_path.replace /\/+/ig, '/'
        part = 'Home' if part == ''
        html += '<li><a href="#" data-path="' + composite_path + '">' + part + '</a></li>'

    $('#path').html html
    $('#path a').click ->
        set_path $(this).attr 'data-path'

    store.listSubdirectories path, (ds) ->
        store.listFiles path, (fs) ->
            $('#entries').html ''
            ds = ds.map (d) -> make_dir_entry(d)
            fs = fs.map (f) -> make_file_entry(f)

            $('#entries').append ds.join('') + '<br>' + fs.join('')

            $('.entry[data-type=file]').each ->
                entry = $(this)
                entry_path = entry.attr('data-path')
                store.fileMetadata entry_path, 'mimetype', (mimetype) ->
                    icon = 'insert_drive_file'

                    if mimetype.startsWith 'video'
                        icon = 'local_movies'
                    else if mimetype.startsWith 'audio'
                        icon = 'library_music'
                    else if is_text mimetype
                        icon = 'mode_edit'

                    if mimetype.startsWith 'image'
                        entry.find('i').replaceWith '<img class="icon" src="thumb://' + entry.attr('data-path') + '">'
                    else
                        entry.find('i').html icon

                    entry.attr 'data-mimetype', mimetype

                    if mimetype.startsWith 'image'
                        entry.attr 'data-filetype', 'image'
                    else if is_text mimetype
                        entry.attr 'data-filetype', 'text'
                    else
                        entry.attr mimetype.split('/').first()

                    reset_click_listener()

            reset_click_listener()

    $('#left-panel').treeview(true).selectNode path_tree_node[path]

select = (node) ->
    node = $(node)
    node.removeClass 'btn-default'
    node.addClass 'btn-info'
    node.attr 'data-selected', true
    $('#file-decrypt').removeClass 'hidden'

deselect = (node) ->
    node = $(node)
    node.removeClass 'btn-info'
    node.addClass 'btn-default'
    node.attr 'data-selected', false
    if $('.entry[data-selected=true]').size() == 0
        $('#file-decrypt').addClass 'hidden'

reset_click_listener = ->
    $('.entry').unbind 'dblclick'
    $('.entry').unbind 'click'

    $('.entry[data-type=folder]').dblclick ->
        set_path $(this).attr 'data-path'

    $('.entry[data-filetype=image]').dblclick open_image_view

    $('.entry[data-filetype=text]').dblclick open_text_view

    $('.entry').click (e) ->
        self = $(this)

        if e.ctrlKey || e.metaKey
            if self.attr('data-selected') == "true"
                deselect self
            else
                select self
        else if e.shiftKey
            entries = $('.entry')
            self_index = entries.index self
            target_index = entries.index $('.entry[data-selected=true]').first()
            first = if self_index < target_index then self_index else target_index
            last = if self_index > target_index then self_index else target_index
            if first == -1 || last == -1
                select self
                return
            entries = entries.slice first, last + 1
            select $(entries)
        else
            deselect $('.entry')
            if self.attr('data-selected') == "true"
                deselect self
            else
                select self

folderTree = (_path) ->
    node_name = _path.split('/').last()
    node_name = 'Home' if node_name == ''
    node =
        text: node_name
        path: _path
        href: '#'
        state:
            selected: _path == path
        nodes: []
    store.listSubdirectories _path, (res) ->
        node.nodes = res.map folderTree
        $('#left-panel').treeview tree_opts
        update_tree_flat_struct()
    node

tree_opts =
    data: []
    color: 'white'
    nodeIcon: 'glyphicon glyphicon-folder-open'
    selectedIcon: 'glyphicon glyphicon-folder-open'
    color: 'white'
    onhoverColor: 'transparent'
    backColor: 'transparent'
    selectedBackColor: 'transparent'
    searchResultBackColor: 'transparent'
    selectedColor: '#009688'
    showBorder: false
    onNodeUnselected: (event, node) ->
        if $('#left-panel').treeview(true).getSelected().length == 0
            setTimeout ->
                if $('#left-panel').treeview(true).getSelected().length == 0
                    $('#left-panel').treeview(true).selectNode node.nodeId
            , 50
    onNodeSelected: (event, node) ->
        set_path node.path

update_tree = ->
    tree_opts.data = [ folderTree '/' ]
    $('#left-panel').treeview tree_opts

update_tree_flat_struct = ->
    path_tree_node = {}
    counter = 0
    store.listAllDirectories (res) ->
        $(res).each ->
            node = $('#left-panel').treeview(true).getNode(counter)
            path_tree_node[node.path] = node
            counter++

make_dir_entry = (path) ->
    name = path.split('/').last()
    name = $('#hidden-div').text(name).html()
    '<div class="entry btn btn-default btn-raised" data-type="folder" data-path="' + path + '"><i class="icon material-icons">folder</i><span class="name">' + name + '</span></div>'

make_file_entry = (path) ->
    name = path.split('/').last()
    name = $('#hidden-div').text(name).html()
    '<div class="entry btn btn-default btn-raised" data-type="file" data-path="' + path + '"><i class="icon material-icons"></i><span class="name">' + name + '</span></div>'

basename = (path) ->
    path.split('/').last()

toast_hide = time()
setInterval (-> $('#toast').hide() if time() > toast_hide ), 5000
toast = (msg) ->
    $('#toast').html msg
    $('#toast').show()
    toast_hide = time() + 5000

# Thanks to http://stackoverflow.com/questions/123999/how-to-tell-if-a-dom-element-is-visible-in-the-current-viewport/7557433#7557433
isElementInViewport = (el) ->
    if typeof jQuery == "function" && el instanceof jQuery
        el = el[0];
    rect = el.getBoundingClientRect();
    rect.top >= 0 &&
    rect.left >= 0 &&
    rect.bottom <= (window.innerHeight || $(window).height()) &&
    rect.right <= (window.innerWidth || $(window).width())

navigate_up = ->
    parent = path.split('/').slice(0, -1).join('/') || '/'
    set_path parent

zoom_factor = 1
current_image = null
open_image_view = ->
    image_entry = $ $('.entry[data-selected=true]').filter('[data-filetype=image]').first() || $('[data-filetype=image]').first()
    return unless image_entry?
    $('#image-view #image').attr 'src', 'decrypt://' + image_entry.attr 'data-path'
    $('#image-view').show()
    current_image = image_entry

set_zoom = (zoom) ->
    width = Math.ceil $('#image-view #image').get(0).naturalWidth
    height = Math.ceil $('#image-view #image').get(0).naturalHeight

    if zoom > 0 && zoom <= 5
        zoom_factor = zoom
    else
        zoom_factor = -1
        cw = $('#image-view #image-container').width()
        ch = $('#image-view #image-container').height()
        zoom = Math.min cw / width, ch / height

    width *= zoom
    height *= zoom

    $('#image-view #image').css 'width', width + 'px'
    $('#image-view #image').css 'height', height + 'px'

open_file_path = null
open_text_view = ->
    entry = $ $('.entry[data-selected=true]').filter('[data-filetype=text]').first() || $('[data-filetype=text]').first()
    $('#text-editor-container').show()
    type = is_text entry.attr 'data-mimetype'
    editor.getSession().setMode 'ace/mode/' + type
    $('#editor-lang').val type
    open_file_path = entry.attr 'data-path'
    store.decryptFile entry.attr('data-path'), (content) ->
        editor.setValue content
        editor.clearSelection()
        editor.gotoLine 1, 0, true
        editor.focus()
    editor_mode_bindings()

decrypt_entries = ->
    entries = $('.entry[data-selected=true]')
    paths = entries.map -> $(this).attr 'data-path'
    sb.decrypt paths.get(), path

set_shortcuts = ->
    $(document).unbind 'keydown'

    $('#file-decrypt').click decrypt_entries

    right_panel_shown = true
    right_panel_width = $('#right-panel').css 'width'
    $('#info-toggle').click ->
        if right_panel_shown
            $('#right-panel').hide 200
            $('#content').css 'right', 0
            right_panel_shown = false
        else
            $('#right-panel').show 200
            $('#content').css 'right', right_panel_width
            right_panel_shown = true

    view = 'grid'
    $('#grid-toggle').click ->
        if view == 'grid'
            $('#grid-toggle i').html 'view_module'
            $('#entries').removeClass 'grid-view'
            $('#entries').addClass 'list-view'
            view = 'list'
        else
            $('#grid-toggle i').html 'view_list'
            $('#entries').addClass 'grid-view'
            $('#entries').removeClass 'list-view'
            view = 'grid'

    $('#add-file').click ->
        $('#add-modal').modal 'hide'
        sb.getFile (fs) ->
            for f in fs
                file_name = basename f
                target_path = path + '/' + file_name
                target_path = target_path.replace(/\/+/ig, '/')
                sb.asyncAddFile f, target_path, ->
                    setTimeout update_views, 1000

    $('#add-folder').click ->
        $('#add-modal').modal 'hide'
        sb.getFolder (folder) ->
            folder_name = basename folder

            target_folder = path + '/' + folder_name
            target_folder = target_folder.replace(/\/+/ig, '/')
            sb.listFilesInFolder folder, (fs) ->
                for f in fs
                    file_store_path = f.replace(folder, target_folder).replace(/\/+/ig,'/')
                    sb.asyncAddFile f, file_store_path, ->
                        setTimeout update_views, 1000

    $('#remove-selected').click ->
        $('#remove-modal').modal 'hide'
        for entry in $('.entry[data-selected=true]')
            entry_path = $(entry).attr 'data-path'
            store.remove entry_path, ->
                toast entry_path + ' removed'
                setTimeout update_views, 1000

    $('#zoom-fit').click ->
        set_zoom -1

    $('#zoom-original').click ->
        set_zoom 1

    $('#zoom-in').click ->
        set_zoom zoom_factor + 0.05

    $('#zoom-out').click ->
        set_zoom zoom_factor - 0.05

    $('#image-view #next').click ->
        images = $('.entry[data-filetype=image]')
        index = ( ( (images.index(current_image) + 1) % images.size()) + images.size() ) % images.size()
        $('#image-view #image').attr 'src', 'decrypt://' + $(images[index]).attr 'data-path'
        current_image = $ images[index]

    $('#image-view #prev').click ->
        images = $('.entry[data-filetype=image]')
        index = ( ( (images.index(current_image) - 1) % images.size()) + images.size() ) % images.size()
        $('#image-view #image').attr 'src', 'decrypt://' + $(images[index]).attr 'data-path'
        current_image = $ images[index]

    $('#image-view #image').load ->
        set_zoom zoom_factor

    $('#text-editor-save').click ->
        content = editor.getValue()
        store.remove open_file_path, ->
            store.addFileFromData open_file_path, content, ->
                toast window.tr 'FileSaved'

    $(document).bind 'keydown', 'ctrl+0', ->
        $('#zoom-original').click()

    $(document).bind 'keydown', 'ctrl+9', ->
        $('#zoom-fit').click()

    $(document).bind 'keydown', 'ctrl++', ->
        $('#zoom-in').click()

    $(document).bind 'keydown', 'ctrl+=', ->
        $('#zoom-in').click()

    $(document).bind 'keydown', 'ctrl+-', ->
        $('#zoom-out').click()

    $(document).bind 'keydown', 'meta+0', ->
        $('#zoom-original').click()

    $(document).bind 'keydown', 'meta+9', ->
        $('#zoom-fit').click()

    $(document).bind 'keydown', 'meta++', ->
        $('#zoom-in').click()

    $(document).bind 'keydown', 'meta+=', ->
        $('#zoom-in').click()

    $(document).bind 'keydown', 'meta+-', ->
        $('#zoom-out').click()

    $(document).bind 'keydown', 'ctrl+a', ->
        deselect $('.entry')

    $(document).bind 'keydown', 'meta+a', ->
        select $('.entry')

    $(document).bind 'keydown', 'ctrl+d', decrypt_entries

    $(document).bind 'keydown', 'meta+d', decrypt_entries

    $(document).keydown (e) ->
        switch e.keyCode
            when 46 # Delete
                $('#remove-modal').modal 'show'
            when 8 # Backspace
                navigate_up()
            when 27 # Esc
                current_image = null
                open_file_path = null
                $('#text-editor-container').hide()
                $('#image-view').hide()
                $('.modal').modal 'hide'
                deselect $('.entry')
            when 13 # Enter
                $('.entry[data-selected=true]').first()?.dblclick()
            when 37 # Left
                if current_image?
                    $('#image-view #prev').click()
                    return
                entries = $('.entry')
                first_selected = $('.entry[data-selected=true]').first() || entries.first()
                index = ( ( (entries.index(first_selected) - 1) % entries.size()) + entries.size() ) % entries.size()
                deselect entries
                select entries.get(index)
                unless isElementInViewport entries.get(index)
                    $('.entry[data-selected=true]').get(0).scrollIntoView()
                    $('#content').scrollTop $('#content').scrollTop() - 90
                    # Prevents last row from getting clipped
                    unless isElementInViewport entries.get(index)
                        $('.entry[data-selected=true]').get(0).scrollIntoView()
            when 38 # Up
                entries = $('.entry')
                first_selected = $('.entry[data-selected=true]').first() || entries.first()
                e = $(entries.first())
                elements_per_row = Math.floor $('#entries').width() / ( e.outerWidth() + e.innerWidth() - e.width() )
                elements_per_row = elements_per_row || 1
                index = ( ( (entries.index(first_selected) - elements_per_row) % entries.size()) + entries.size() ) % entries.size()
                deselect entries
                select entries.get(index)
                unless isElementInViewport entries.get(index)
                    $('.entry[data-selected=true]').get(0).scrollIntoView()
                    $('#content').scrollTop $('#content').scrollTop() - 90
                    # Prevents last row from getting clipped
                    unless isElementInViewport entries.get(index)
                        $('.entry[data-selected=true]').get(0).scrollIntoView()
            when 39 # Right
                if current_image?
                    $('#image-view #next').click()
                    return
                entries = $('.entry')
                first_selected = $('.entry[data-selected=true]').first()
                index = ( ( (entries.index(first_selected) + 1) % entries.size()) + entries.size() ) % entries.size()
                deselect entries
                select entries.get(index)
                unless isElementInViewport entries.get(index)
                    $('.entry[data-selected=true]').get(0).scrollIntoView()
                    $('#content').scrollTop $('#content').scrollTop() - 90
                    # Prevents last row from getting clipped
                    unless isElementInViewport entries.get(index)
                        $('.entry[data-selected=true]').get(0).scrollIntoView()
            when 40 # Down
                entries = $('.entry')
                first_selected = $('.entry[data-selected=true]').first() || entries.first()
                e = $(entries.first())
                elements_per_row = Math.floor $('#entries').width() / ( e.outerWidth() + e.innerWidth() - e.width() )
                elements_per_row = elements_per_row || 1
                index = ( ( (entries.index(first_selected) + elements_per_row) % entries.size()) + entries.size() ) % entries.size()
                deselect entries
                select entries.get(index)
                unless isElementInViewport entries.get(index)
                    $('.entry[data-selected=true]').get(0).scrollIntoView()
                    $('#content').scrollTop $('#content').scrollTop() - 90
                    # Prevents last row from getting clipped
                    unless isElementInViewport entries.get(index)
                        $('.entry[data-selected=true]').get(0).scrollIntoView()

editor_mode_bindings = ->
    $(document).unbind 'keydown'
    $(document).keydown (e) ->
        if e.keyCode == 27 # Esc
            current_image = null
            open_file_path = null
            $('#text-editor-container').hide()
            $('#image-view').hide()
            $('.modal').modal 'hide'
            deselect $('.entry')
            set_shortcuts()
$ ->
    $.material.init()

    $('#toast').hide()
    $('#image-view').hide()
    $('#text-editor-container').hide()

    editor = ace.edit 'text-editor'
    editor.setTheme 'ace/theme/ambiance'

    $('#editor-lang').append '<option>' + lang + '</option>' for lang in supported_languages
    $('#editor-lang').change ->
        editor.getSession().setMode 'ace/mode/' + $(this).val()

    $('#editor-theme').append '<option>' + theme + '</option>' for theme in ace_themes
    $('#editor-theme').change ->
        editor.setTheme 'ace/theme/' + $(this).val()
        sb.saveSetting 'ace-theme', $(this).val()

    set_shortcuts()

    wcp.done ->
        $(window.trs).each ->
            locale = this
            $('#' + locale).click ->
                window.locale = locale
                window.update_translation()
                sb.setLang locale

        sb.lang (r) ->
            window.locale = r
            window.update_translation()

        sb.startAddFile.connect (fsPath, storePath) ->
            toast window.tr('Adding') + ' ' + storePath

        sb.endAddFile.connect (fsPath, storePath) ->
            toast window.tr('Added') + ' ' + storePath
            update_tree()
            set_path '/'

        sb.startDecryptFile.connect (path) ->
            toast window.tr('Decrypting') + ' ' + path

        sb.startDecryptFile.connect (path) ->
            toast window.tr('Decrypted') + ' ' + path

        sb.setting 'ace-theme', (theme) ->
            if theme
                editor.setTheme 'ace/theme/' + theme
                $('#editor-theme').val theme

        update_tree()
        set_path '/'

    $(window).on
        dragover: -> false
        dragend: -> false
        drop: -> false

wcp = do ->
    deferred = new $.Deferred

    new QWebChannel qt.webChannelTransport, (channel) ->
        sb = channel.objects.store_bridge
        store = channel.objects.store
        deferred.resolve()

    deferred.promise()
