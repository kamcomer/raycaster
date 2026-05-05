import pygame
import pygame_gui
import os
from event_bus import EventBus

from screen_area import ScreenArea
from tkinter import filedialog
from PIL import Image

OPTION_BTN_SIZE = (70, 30)


class OptionsBar:
    def __init__(
        self,
        area: ScreenArea,
        manager: pygame_gui.UIManager,
        event_bus: EventBus | None,
    ) -> None:
        self._area = area
        self._manager = manager

        self.elements = {}

        if event_bus:
            self.subscribe_to_events(event_bus)

        self._init_bar()

    def subscribe_to_events(self, event_bus: EventBus):
        self._event_bus = event_bus

    def _init_bar(self):
        self.root_panel = pygame_gui.elements.UIPanel(
            self.area.abs_rect,
            manager=self._manager,
            element_id="options_bar",
        )
        self.elements["root"] = self.root_panel

        save_btn = pygame_gui.elements.UIButton(
            pygame.Rect(0, 0, *OPTION_BTN_SIZE),
            "Save",
            self._manager,
            container=self.root_panel,
            anchors={"left": "left", "top": "top"},
        )
        self.elements["btn_save"] = save_btn
        if self._event_bus:
            self._event_bus.subscribe_element(
                pygame_gui.UI_BUTTON_PRESSED, save_btn, self.handle_save
            )

        load_btn = pygame_gui.elements.UIButton(
            pygame.Rect(0, 0, 70, 30),
            "Load",
            self._manager,
            container=self.root_panel,
            anchors={"left_target": save_btn, "top": "top"},
        )
        self.elements["btn_load"] = load_btn
        if self._event_bus:
            self._event_bus.subscribe_element(
                pygame_gui.UI_BUTTON_PRESSED, load_btn, self.handle_load
            )

        import_btn = pygame_gui.elements.UIButton(
            pygame.Rect(0, 0, 70, 30),
            "Import",
            self._manager,
            container=self.root_panel,
            anchors={"left_target": load_btn, "top": "top"},
        )
        self.elements["btn_import"] = import_btn
        if self._event_bus:
            self._event_bus.subscribe_element(
                pygame_gui.UI_BUTTON_PRESSED, import_btn, self.handle_import
            )

        export_btn = pygame_gui.elements.UIButton(
            pygame.Rect(0, 0, 70, 30),
            "Export",
            self._manager,
            container=self.root_panel,
            anchors={"left_target": import_btn, "top": "top"},
        )
        self.elements["btn_export"] = export_btn
        if self._event_bus:
            self._event_bus.subscribe_element(
                pygame_gui.UI_BUTTON_PRESSED, export_btn, self.handle_export
            )

    def _create_button(self, name, text, container, anchors, callback=None):
        btn = pygame_gui.elements.UIButton(
            pygame.Rect((0, 0), OPTION_BTN_SIZE),
            text,
            self._manager,
            container,
            anchors=anchors,
        )
        self.elements[f"btn_{name.lower()}"] = btn
        if callback and self._event_bus:
            self._event_bus.subscribe_element(
                pygame_gui.UI_BUTTON_PRESSED, btn, callback
            )
        return btn

    def pil_to_surface(self, pil_image):
        """Converts a PIL image to a Pygame surface."""
        data = pil_image.tobytes()
        size = pil_image.size
        mode = pil_image.mode

        return pygame.image.frombytes(data, size, mode).convert_alpha()

    def handle_import(self):
        print("Import")

    def handle_load(self):
        print("Load")

    def handle_export(self):
        print("Export")

    def handle_save(self):
        print("Save")

    @property
    def area(self):
        return self._area

    @area.setter
    def area(self, value: ScreenArea):
        self._area = value

    @property
    def root_element(self):
        return self.elements.get("root")

    @property
    def manager(self):
        return self._manager
