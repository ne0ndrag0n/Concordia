bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "entity" )

local modpack_path = ...
local GUIProvider = bluebear.extend( 'system.entity.base', 'system.provider.gui' )

function GUIProvider:open_debug_ui()
  local path = modpack_path.."/debug.xml"
  print( "system.provider.gui", "Providing the debug UI from "..path )

  self.gui = bluebear.gui.create_gui_context( path )

  self.gui:find_by_id( "rot_l" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_rot_l", self ) )
  self.gui:find_by_id( "rot_r" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_rot_r", self ) )
  self.gui:find_by_id( "zoom_in" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_zoom_in", self ) )
  self.gui:find_by_id( "zoom_out" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_zoom_out", self ) )

  self.gui:find_by_id( "misc" ):on( "click", bluebear.util.bind( "system.provider.gui:do_something", self ) )

  -- XXX: Remove after demo
  self.gui:find_by_id( "animate1" ):on( "click", bluebear.gui.__internal__playanim1 )
end

function GUIProvider:do_something( event )
  local notebook = self.gui:find_by_id( 'notebook' )

  local page_string = [[
    <page>
      <tab>
        <Label id="label_new">New Tab!</Label>
      </tab>
      <content>
        <Box id="boxid">
          <Label id="contentlabel">Content</Label>
        </Box>
      </content>
    </page>
  ]]

  notebook:add( page_string )
end

function GUIProvider:on_click_zoom_in()
  bluebear.gui.zoom_in()
end

function GUIProvider:on_click_zoom_out()
  bluebear.gui.zoom_out()
end

function GUIProvider:on_click_rot_l()
  bluebear.gui.rotate_left()
end

function GUIProvider:on_click_rot_r()
  bluebear.gui.rotate_right()
end

bluebear.register_class( GUIProvider )
