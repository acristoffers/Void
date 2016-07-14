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
  var basename, deselect, folderTree, make_dir_entry, make_file_entry, path, path_tree_node, reset_click_listener, sb, select, set_path, store, time, toast, toast_hide, tree_opts, update_tree, update_tree_flat_struct, update_views, wcp;

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

  path = '';

  path_tree_node = {};

  time = function() {
    return new Date().getTime();
  };

  update_views = function() {
    set_path(path);
    return update_tree();
  };

  set_path = function(new_path) {
    var composite_path, html, i, len, part, path_parts;
    path = new_path || '';
    path_parts = path.split('/');
    if (path_parts.last() === '') {
      path_parts = [''];
    }
    composite_path = '';
    html = '';
    for (i = 0, len = path_parts.length; i < len; i++) {
      part = path_parts[i];
      composite_path += '/' + part;
      composite_path = composite_path.replace(/\/+/ig, '/');
      if (part === '') {
        part = 'Home';
      }
      html += '<li><a href="#" data-path="' + composite_path + '">' + part + '</a></li>';
    }
    $('#path').html(html);
    $('#path a').click(function() {
      return set_path($(this).attr('data-path'));
    });
    store.listSubdirectories(path, function(ds) {
      return store.listFiles(path, function(fs) {
        $('#entries').html('');
        ds = ds.map(function(d) {
          return make_dir_entry(d);
        });
        fs = fs.map(function(f) {
          return make_file_entry(f);
        });
        $('#entries').append(ds.join('') + '<br>' + fs.join(''));
        $('.entry[data-type=file]').each(function() {
          var entry, entry_path;
          entry = $(this);
          entry_path = entry.attr('data-path');
          return store.fileMetadata(entry_path, 'mimetype', function(mimetype) {
            var icon;
            icon = 'insert_drive_file';
            if (mimetype.startsWith('video')) {
              icon = 'local_movies';
            } else if (mimetype.startsWith('audio')) {
              icon = 'library_music';
            } else if (mimetype.startsWith('text')) {
              icon = 'mode_edit';
            }
            if (mimetype.startsWith('image')) {
              entry.find('i').replaceWith('<img class="icon" src="thumb://' + entry.attr('data-path') + '">');
            } else {
              entry.find('i').html(icon);
            }
            entry.attr('data-mimetype', mimetype);
            return entry.attr('data-filetype', mimetype.split('/').first());
          });
        });
        return reset_click_listener();
      });
    });
    return $('#left-panel').treeview(true).selectNode(path_tree_node[path]);
  };

  select = function(node) {
    node = $(node);
    node.removeClass('btn-default');
    node.addClass('btn-info');
    return node.attr('data-selected', true);
  };

  deselect = function(node) {
    node = $(node);
    node.removeClass('btn-info');
    node.addClass('btn-default');
    return node.attr('data-selected', false);
  };

  reset_click_listener = function() {
    $('.entry').unbind('dblclick');
    $('.entry').unbind('click');
    $('.entry[data-type=folder]').dblclick(function() {
      return set_path($(this).attr('data-path'));
    });
    return $('.entry').click(function(e) {
      var entries, first, last, self, self_index, target_index;
      self = $(this);
      if (e.ctrlKey || e.metaKey) {
        if (self.attr('data-selected') === "true") {
          return deselect(self);
        } else {
          return select(self);
        }
      } else if (e.shiftKey) {
        entries = $('.entry');
        self_index = entries.index(self);
        target_index = entries.index($('.entry[data-selected=true]').first());
        first = self_index < target_index ? self_index : target_index;
        last = self_index > target_index ? self_index : target_index;
        if (first === -1 || last === -1) {
          select(self);
          return;
        }
        entries = entries.slice(first, last + 1);
        return select($(entries));
      } else {
        deselect($('.entry'));
        if (self.attr('data-selected') === "true") {
          return deselect(self);
        } else {
          return select(self);
        }
      }
    });
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
    return '<div class="entry btn btn-default btn-raised" data-type="folder" data-path="' + path + '"><i class="icon material-icons">folder</i><span class="name">' + name + '</span></div>';
  };

  make_file_entry = function(path) {
    var name;
    name = path.split('/').last();
    name = $('#hidden-div').text(name).html();
    return '<div class="entry btn btn-default btn-raised" data-type="file" data-path="' + path + '"><i class="icon material-icons"></i><span class="name">' + name + '</span></div>';
  };

  basename = function(path) {
    return path.split('/').last();
  };

  toast_hide = time();

  setInterval((function() {
    if (time() > toast_hide) {
      return $('#toast').hide();
    }
  }), 5000);

  toast = function(msg) {
    $('#toast').html(msg);
    $('#toast').show();
    return toast_hide = time() + 5000;
  };

  $(function() {
    var right_panel_shown, right_panel_width, view;
    $.material.init();
    $('#toast').hide();
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
      sb.startAddFile.connect(function(fsPath, storePath) {
        return toast('Adding ' + storePath);
      });
      sb.endAddFile.connect(function(fsPath, storePath) {
        toast('Added ' + storePath);
        update_tree();
        return set_path('/');
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
    $('#grid-toggle').click(function() {
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
    $('#add-file').click(function() {
      $('#add-modal').modal('hide');
      return sb.getFile(function(fs) {
        var f, file_name, i, len, results, target_path;
        results = [];
        for (i = 0, len = fs.length; i < len; i++) {
          f = fs[i];
          file_name = basename(f);
          target_path = path + '/' + file_name;
          target_path = target_path.replace(/\/+/ig, '/');
          results.push(sb.asyncAddFile(f, target_path, function() {
            return setTimeout(update_views, 1000);
          }));
        }
        return results;
      });
    });
    $('#add-folder').click(function() {
      $('#add-modal').modal('hide');
      return sb.getFolder(function(folder) {
        var folder_name, target_folder;
        folder_name = basename(folder);
        target_folder = path + '/' + folder_name;
        target_folder = target_folder.replace(/\/+/ig, '/');
        return sb.listFilesInFolder(folder, function(fs) {
          var f, file_store_path, i, len, results;
          results = [];
          for (i = 0, len = fs.length; i < len; i++) {
            f = fs[i];
            file_store_path = f.replace(folder, target_folder).replace(/\/+/ig, '/');
            results.push(sb.asyncAddFile(f, file_store_path, function() {
              return setTimeout(update_views, 1000);
            }));
          }
          return results;
        });
      });
    });
    $('#remove-selected').click(function() {
      var entry, entry_path, i, len, ref, results;
      $('#remove-modal').modal('hide');
      ref = $('.entry[data-selected=true]');
      results = [];
      for (i = 0, len = ref.length; i < len; i++) {
        entry = ref[i];
        entry_path = $(entry).attr('data-path');
        results.push(store.remove(entry_path, function() {
          toast(entry_path + ' removed');
          return setTimeout(update_views, 1000);
        }));
      }
      return results;
    });
    $(document).bind('keydown', 'ctrl+a', function() {
      return deselect($('.entry'));
    });
    $(document).bind('keydown', 'meta+a', function() {
      return select($('.entry'));
    });
    $(document).keydown(function(e) {
      switch (e.keyCode) {
        case 46:
          return $('#remove-modal').modal('show');
        case 8:
          return $('#remove-modal').modal('show');
        case 27:
          $('.modal').modal('hide');
          return deselect($('.entry'));
        case 13:
          return $('.entry[data-selected=true]').first().dblclick();
      }
    });
    return $(window).on({
      dragover: function() {
        return false;
      },
      dragend: function() {
        return false;
      },
      drop: function() {
        return false;
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
