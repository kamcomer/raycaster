import sys
import tkinter as tk
from tkinter import filedialog
from typing import Any
import os

import pygame
import pygame_gui
from PIL import Image
from pygame.image import save

from layout_manager import LayoutManager
from map_editor import MapEditor
from screen_area import ScreenArea, ScreenAreaBounds
from texture_pallet import TexturePallet

DEFAULT_SCREEN_SIZE = (800, 450)
DEFAULT_MAP_AREA_SIZE = (650, 450)
DEFAULT_PALLET_AREA_SIZE = (150, 450)

DEFAULT_MAP_GRID_SIZE = (24, 24)
TICK_RATE = 60


class OptionsBar:
    def __init__(self, area: ScreenArea, manager: pygame_gui.UIManager) -> None:
        self._area = area
        self._manager = manager

        self._init_bar()

    def _init_bar(self):
        # panel = pygame_gui.elements.UIPanel(self.area.abs_rect, manager=self.manager)
        save_btn = pygame_gui.elements.UIButton(
            pygame.Rect(0, 0, 70, 30), "Save", self._manager
        )
        load_btn = pygame_gui.elements.UIButton(
            pygame.Rect(0, 0, 70, 30),
            "Load",
            self._manager,
            anchors={"left_target": save_btn},
        )
        import_btn = pygame_gui.elements.UIButton(
            pygame.Rect(0, 0, 70, 30),
            "Import",
            self._manager,
            anchors={"left_target": load_btn},
        )
        export_btn = pygame_gui.elements.UIButton(
            pygame.Rect(0, 0, 70, 30),
            "Export",
            self._manager,
            anchors={"left_target": import_btn},
        )

    def pil_to_surface(self, pil_image):
        """Converts a PIL image to a Pygame surface."""
        # Use tobytes() to get the raw pixel data
        data = pil_image.tobytes()
        size = pil_image.size
        mode = pil_image.mode  # Typically 'RGB' or 'RGBA'

        # Create the surface and convert it for optimized performance
        return pygame.image.frombytes(data, size, mode).convert_alpha()

    def import_texture(self):
        file_path = filedialog.askopenfilename()
        if not file_path:
            return

        file_name = os.path.basename(file_path)
        image = Image.open(file_path)
        img_surf = self.pil_to_surface(image)
        print(img_surf)
        # pygame_gui.elements.UIImage(
        #     pygame.Rect(32, 200, 128, 128),
        #     img_surf,
        #     self._manager,
        #     container=self.root_element,
        # )

    @property
    def area(self):
        return self._area

    @property
    def manager(self):
        return self._manager


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

    event_handlers = {
        pygame.QUIT: [],
        pygame.WINDOWRESIZED: [layout_manager.handle_resize],
        pygame.MOUSEBUTTONDOWN: [],
        pygame_gui.UI_BUTTON_PRESSED: [],
    }

    map = MapEditor(DEFAULT_MAP_GRID_SIZE, map_area, event_handlers)
    texture_pallet = TexturePallet(texture_pallet_area, manager, event_handlers)
    options_bar = OptionsBar(
        options_area,
        manager,
    )

    while True:
        dt = clock.tick(TICK_RATE) / 1000.0
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

            if event.type == pygame.WINDOWRESIZED:
                manager.set_window_resolution((event.x, event.y))

            if event.type in event_handlers:
                for callback in event_handlers[event.type]:
                    callback(event)

            manager.process_events(event)

        manager.update(dt)
        # map.update()
        # options_area.blit(screen)
        # texture_pallet.update()
        screen.fill("#000000")
        layout_manager.blit()
        manager.draw_ui(screen)
        pygame.display.flip()


if __name__ == "__main__":
    main()
