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

time = ->
    new Date().getTime()

update_views = ->
    set_path path
    update_tree()

set_path = (new_path) ->
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
                    else if mimetype.startsWith 'text'
                        icon = 'mode_edit'

                    if mimetype.startsWith 'image'
                        entry.find('i').replaceWith '<img class="icon" src="thumb://' + entry.attr('data-path') + '">'
                    else
                        entry.find('i').html icon
                    entry.attr 'data-mimetype', mimetype
                    entry.attr 'data-filetype', mimetype.split('/').first()

            reset_click_listener()

    $('#left-panel').treeview(true).selectNode path_tree_node[path]

select = (node) ->
    node = $(node)
    node.removeClass 'btn-default'
    node.addClass 'btn-info'
    node.attr 'data-selected', true

deselect = (node) ->
    node = $(node)
    node.removeClass 'btn-info'
    node.addClass 'btn-default'
    node.attr 'data-selected', false

reset_click_listener = ->
    $('.entry').unbind 'dblclick'
    $('.entry').unbind 'click'

    $('.entry[data-type=folder]').dblclick ->
        set_path $(this).attr 'data-path'

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

$ ->
    $.material.init()

    $('#toast').hide()

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
            toast 'Adding ' + storePath

        sb.endAddFile.connect (fsPath, storePath) ->
            toast 'Added ' + storePath
            update_tree()
            set_path '/'

        update_tree()
        set_path '/'

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

    $(document).bind 'keydown', 'ctrl+a', ->
        deselect $('.entry')

    $(document).bind 'keydown', 'meta+a', ->
        select $('.entry')

    $(document).keydown (e) ->
        switch e.keyCode
            when 46 # Delete
                $('#remove-modal').modal 'show'
            when 8 # Backspace
                $('#remove-modal').modal 'show'
            when 27 # Esc
                $('.modal').modal 'hide'
                deselect $('.entry')
            when 13 # Enter
                $('.entry[data-selected=true]').first()?.dblclick()
            when 37 # Left
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
