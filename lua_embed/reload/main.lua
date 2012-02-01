function reload(module_name)
  package.loaded[module_name] = nil
  require(module_name)
end

reload("mod_player")
reload("mod_item")
