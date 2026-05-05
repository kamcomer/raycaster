import pygame_gui
import pygame
from functools import partial
from screen_area import ScreenArea
from event_bus import EventBus

ACTION_PANEL_HEIGHT = 60
ACTION_CONTAINER_HEIGHT = 50
ACTION_BUTTON_SIZE = (70, 25)
TEXTURE_ITEM_SPACING = 35
TEXTURE_ITEM_HEIGHT = 30
NUM_TEXTURE_ITEMS = 20


class TexturePallet:
    def __init__(
        self,
        area: ScreenArea,
        manager: pygame_gui.UIManager,
        event_bus: EventBus | None = None,
    ) -> None:
        self._area = area
        self._manager = manager
        self.textures = []
        self.sprites = []

        self.elements = {}

        if event_bus is not None:
            self.subscribe_to_events(event_bus)

        self._init_gui()

    def get_element(self, name):
        return self.elements[name]

    def subscribe_to_events(self, event_bus: EventBus):
        self._event_bus = event_bus
        event_bus.subscribe(pygame.WINDOWRESIZED, self.handle_resize)

    def _init_gui(self):
        root_element = pygame_gui.elements.UIPanel(
            self.area.abs_rect,
            manager=self._manager,
            element_id="texture_pallet",
        )
        self.elements["root"] = root_element
        # self._create_action_panel()
        self._create_texture_viewer()

    def _create_texture_viewer(self):
        root_size = self.root_element.get_container().get_size()

        container = pygame_gui.elements.UIScrollingContainer(
            pygame.Rect((0, 0), root_size),
            manager=self._manager,
            object_id=pygame_gui.core.ObjectID(object_id="#scroll_container"),
            container=self.root_element,
            anchors={
                "left": "left",
                "right": "right",
                "top": "top",
                "bottom": "bottom",
            },
        )
        container.set_scrollable_area_dimensions(self._area.size)
        container_width = container.get_container().get_size()[0]
        for i in range(NUM_TEXTURE_ITEMS):
            btn = pygame_gui.elements.UIButton(
                relative_rect=pygame.Rect(
                    (0, i * TEXTURE_ITEM_SPACING),
                    (container_width, TEXTURE_ITEM_HEIGHT),
                ),
                text=f"Item {i + 1}",
                manager=self._manager,
                container=container,
                anchors={"left": "left", "right": "right"},
            )
            self.elements[f"btn_{i}"] = btn
            if self._event_bus:
                self._event_bus.subscribe_element(
                    pygame_gui.UI_BUTTON_PRESSED, btn, partial(self._on_texture_item_clicked, i)
                )
        self.elements["texture_container"] = container

    def _create_button(self, name, text, container, anchors, callback=None):
        btn = pygame_gui.elements.UIButton(
            pygame.Rect((0, 0), ACTION_BUTTON_SIZE),
            text,
            self._manager,
            container,
            anchors=anchors,
        )
        self.add_element(f"btn_{name.lower()}", btn, callback)
        return btn

    def _on_texture_item_clicked(self, index):
        print(f"btn_{index}")

    def handle_resize(self, event):
        self.elements["root"].set_relative_position(self._area.pos)
        self.elements["root"].set_dimensions(self._area.size)

        container = self.elements.get("texture_container")
        if container:
            container_size = container.get_container().get_size()
            container.set_scrollable_area_dimensions(container_size)

    def save(self):
        print("save")

    def load(self):
        print("load")

    def export(self):
        print("export")

    def update(self):
        pass

    @property
    def area(self):
        return self._area

    @area.setter
    def area(self, value: ScreenArea):
        self._area = value

    @property
    def root_element(self):
        return self.elements["root"]
