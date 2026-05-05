import sys
import tkinter as tk

import pygame
import pygame_gui

from options_bar import OptionsBar
from layout_manager import LayoutManager
from map_editor import MapEditor
from screen_area import ScreenAreaBounds
from texture_pallet import TexturePallet
from event_bus import EventBus

DEFAULT_SCREEN_SIZE = (800, 450)
DEFAULT_MAP_AREA_SIZE = (650, 450)
DEFAULT_PALLET_AREA_SIZE = (150, 450)

DEFAULT_MAP_GRID_SIZE = (24, 24)
TICK_RATE = 60


def main():
    root = tk.Tk()
    root.withdraw()
    screen_size = DEFAULT_SCREEN_SIZE

    pygame.init()
    screen = pygame.display.set_mode(screen_size, pygame.RESIZABLE)
    clock = pygame.time.Clock()

    manager = pygame_gui.UIManager(screen_size, theme_path="theme.json")

    layout_manager = LayoutManager(screen)
    options_area = layout_manager.create_area(
        (0.8, 0.00),
        area_bounds=ScreenAreaBounds(y_min=30),
        bg_color=pygame.Color("#ff0000"),
    )
    map_area = layout_manager.create_area(
        (0.8, 0.95),
        (0, 0.05),
        bg_color=pygame.Color("#000000"),
    )
    texture_pallet_area = layout_manager.create_area(
        (0.2, 1), (0.8, 0), bg_color=pygame.Color("#000000")
    )

    event_bus = EventBus()
    event_bus.subscribe(pygame.WINDOWRESIZED, layout_manager.handle_resize)

    map = MapEditor(DEFAULT_MAP_GRID_SIZE, map_area, event_bus)
    texture_pallet = TexturePallet(texture_pallet_area, manager, event_bus)
    options_bar = OptionsBar(options_area, manager, event_bus)

    while True:
        dt = clock.tick(TICK_RATE) / 1000.0
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

            if event.type == pygame.WINDOWRESIZED:
                manager.set_window_resolution((event.x, event.y))

            event_bus.dispatch(event)

            manager.process_events(event)

        manager.update(dt)
        screen.fill("#000000")
        layout_manager.blit()
        manager.draw_ui(screen)
        pygame.display.flip()


if __name__ == "__main__":
    main()
