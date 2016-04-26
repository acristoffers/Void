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
path = '/'
path_tree_node = {}

set_path = (new_path) ->
    path = new_path
    path_parts = path.split '/'
    path_parts = [''] if path_parts.last() == ''
    composite_path = ''
    $('#path').html ''

    for part in path_parts
        composite_path += '/' + part
        composite_path = composite_path.replace '//', '/'
        part = 'Home' if part == ''
        $('#path').append '<li><a href="#" data-path="' + composite_path + '">' + part + '</a></li>'

    $('#path a').click ->
        set_path $(this).attr 'data-path'

    $('#left-panel').treeview(true).selectNode path_tree_node[path]

wcp = do ->
    deferred = new $.Deferred

    new QWebChannel qt.webChannelTransport, (channel) ->
        sb = channel.objects.store_bridge
        store = channel.objects.store
        deferred.resolve()

    deferred.promise()

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

$ ->
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

        update_tree()
        set_path '/'

    right_panel_shown = true
    $('#info-toggle').click ->
        if right_panel_shown
            $('#right-panel').hide()
            $('#content').css 'right', 0
            right_panel_shown = false
        else
            $('#right-panel').show()
            $('#content').css 'right', $('#right-panel').css 'width'
            right_panel_shown = true
