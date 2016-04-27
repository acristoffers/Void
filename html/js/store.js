// Generated by CoffeeScript 1.10.0

/*
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
 */

(function() {
  var folderTree, make_dir_entry, path, path_tree_node, sb, set_path, store, tree_opts, update_tree, update_tree_flat_struct, wcp;

  if (!Array.prototype.last) {
    Array.prototype.last = function() {
      return this[this.length - 1];
    };
  }

  if (!Array.prototype.first) {
    Array.prototype.first = function() {
      return this[0];
    };
  }

  sb = null;

  store = null;

  path = '/';

  path_tree_node = {};

  set_path = function(new_path) {
    var composite_path, i, len, part, path_parts;
    path = new_path;
    path_parts = path.split('/');
    if (path_parts.last() === '') {
      path_parts = [''];
    }
    composite_path = '';
    $('#path').html('');
    for (i = 0, len = path_parts.length; i < len; i++) {
      part = path_parts[i];
      composite_path += '/' + part;
      composite_path = composite_path.replace('//', '/');
      if (part === '') {
        part = 'Home';
      }
      $('#path').append('<li><a href="#" data-path="' + composite_path + '">' + part + '</a></li>');
    }
    $('#path a').click(function() {
      return set_path($(this).attr('data-path'));
    });
    store.listSubdirectories(path, function(es) {
      $('#entries').html('');
      $(es).each(function() {
        return $('#entries').append(make_dir_entry(this));
      });
      $('#entries .entry[data-type=folder]').dblclick(function() {
        return set_path($(this).attr('data-path'));
      });
      return $('#entries .entry').click(function() {
        var self;
        self = $(this);
        $('#entries .entry').removeClass('btn-info');
        if (self.attr('data-selected') === "true") {
          self.addClass('btn-default');
          return self.attr('data-selected', false);
        } else {
          self.removeClass('btn-default');
          self.addClass('btn-info');
          $('#entries .entry').attr('data-selected', false);
          return self.attr('data-selected', true);
        }
      });
    });
    return $('#left-panel').treeview(true).selectNode(path_tree_node[path]);
  };

  folderTree = function(_path) {
    var node, node_name;
    node_name = _path.split('/').last();
    if (node_name === '') {
      node_name = 'Home';
    }
    node = {
      text: node_name,
      path: _path,
      href: '#',
      state: {
        selected: _path === path
      },
      nodes: []
    };
    store.listSubdirectories(_path, function(res) {
      node.nodes = res.map(folderTree);
      $('#left-panel').treeview(tree_opts);
      return update_tree_flat_struct();
    });
    return node;
  };

  tree_opts = {
    data: [],
    color: 'white',
    nodeIcon: 'glyphicon glyphicon-folder-open',
    selectedIcon: 'glyphicon glyphicon-folder-open',
    color: 'white',
    onhoverColor: 'transparent',
    backColor: 'transparent',
    selectedBackColor: 'transparent',
    searchResultBackColor: 'transparent',
    selectedColor: '#009688',
    showBorder: false,
    onNodeUnselected: function(event, node) {
      if ($('#left-panel').treeview(true).getSelected().length === 0) {
        return setTimeout(function() {
          if ($('#left-panel').treeview(true).getSelected().length === 0) {
            return $('#left-panel').treeview(true).selectNode(node.nodeId);
          }
        }, 50);
      }
    },
    onNodeSelected: function(event, node) {
      return set_path(node.path);
    }
  };

  update_tree = function() {
    tree_opts.data = [folderTree('/')];
    return $('#left-panel').treeview(tree_opts);
  };

  update_tree_flat_struct = function() {
    var counter;
    path_tree_node = {};
    counter = 0;
    return store.listAllDirectories(function(res) {
      return $(res).each(function() {
        var node;
        node = $('#left-panel').treeview(true).getNode(counter);
        path_tree_node[node.path] = node;
        return counter++;
      });
    });
  };

  make_dir_entry = function(path) {
    var name;
    name = path.split('/').last();
    name = $('#hidden-div').text(name).html();
    path = $('#hidden-div').text(path).html();
    return '<div class="entry btn btn-default btn-raised" data-type="folder" data-path="' + path + '"><i class="icon material-icons">folder</i><span class="name">' + name + '</span></div>';
  };

  $(function() {
    var right_panel_shown, right_panel_width, view;
    $.material.init();
    wcp.done(function() {
      $(window.trs).each(function() {
        var locale;
        locale = this;
        return $('#' + locale).click(function() {
          window.locale = locale;
          window.update_translation();
          return sb.setLang(locale);
        });
      });
      sb.lang(function(r) {
        window.locale = r;
        return window.update_translation();
      });
      update_tree();
      return set_path('/');
    });
    right_panel_shown = true;
    right_panel_width = $('#right-panel').css('width');
    $('#info-toggle').click(function() {
      if (right_panel_shown) {
        $('#right-panel').hide(200);
        $('#content').css('right', 0);
        return right_panel_shown = false;
      } else {
        $('#right-panel').show(200);
        $('#content').css('right', right_panel_width);
        return right_panel_shown = true;
      }
    });
    view = 'grid';
    return $('#grid-toggle').click(function() {
      if (view === 'grid') {
        $('#grid-toggle i').html('view_module');
        $('#entries').removeClass('grid-view');
        $('#entries').addClass('list-view');
        return view = 'list';
      } else {
        $('#grid-toggle i').html('view_list');
        $('#entries').addClass('grid-view');
        $('#entries').removeClass('list-view');
        return view = 'grid';
      }
    });
  });

  wcp = (function() {
    var deferred;
    deferred = new $.Deferred;
    new QWebChannel(qt.webChannelTransport, function(channel) {
      sb = channel.objects.store_bridge;
      store = channel.objects.store;
      return deferred.resolve();
    });
    return deferred.promise();
  })();

}).call(this);
