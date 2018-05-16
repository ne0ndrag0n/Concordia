print( ..., 'This is a modpack' )

-- Load demo components
bluebear.event.register_key( 'v', function()
  local items = bluebear.gui.load_xml( 'system/ui/example.xml' )

  for i, element in ipairs( items ) do
    bluebear.gui.root_element:add_child( element )
  end
end )
