from collections import defaultdict
import pygame_gui
import pygame
from functools import partial
from screen_area import ScreenArea

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
        event_handlers: dict | None = None,
    ) -> None:
        self._area = area
        self._manager = manager
        self.textures = []
        self.sprites = []

        self.elements = {}
        self.element_types = defaultdict(list)
        self.element_callbacks = {}

        if event_handlers is not None:
            self.subscribe_to_events(event_handlers)

        self._init_gui()

    def add_element(self, name, element, callback=None):
        element_type = type(element)
        self.elements[name] = element
        self.element_types[element_type].append(element)
        if callback:
            if element_type not in self.element_callbacks:
                self.element_callbacks[element_type] = {}
            self.element_callbacks[element_type][element] = callback

    def get_element(self, name):
        return self.elements[name]

    def subscribe_to_events(self, event_handlers: dict):
        event_handlers[pygame.WINDOWRESIZED].append(self.handle_resize)
        event_handlers[pygame_gui.UI_BUTTON_PRESSED].append(self.handle_button)

    def _init_gui(self):
        root_element = pygame_gui.elements.UIPanel(
            self.area.abs_rect,
            manager=self._manager,
            element_id="texture_pallet",
        )
        self.add_element("root", root_element)
        # self._create_action_panel()
        self._create_texture_viewer()

    def _create_texture_viewer(self):
        root_size = self.root_element.get_container().get_size()
        # texture_viewer = pygame_gui.elements.UIPanel(
        #     pygame.Rect((0, 0), root_size),
        #     manager=self._manager,
        #     container=self.root_element,
        #     anchors={
        #         "left": "left",
        #         "right": "right",
        #         "top": "top",
        #         "bottom": "bottom",
        #     },
        # )

        container = pygame_gui.elements.UIScrollingContainer(
            pygame.Rect((0, 0), root_size),
            self._manager,
            should_grow_automatically=True,
            object_id=pygame_gui.core.ObjectID(object_id="#scroll_container"),
            container=self.root_element,
            allow_scroll_x=False,
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
            self.add_element(f"btn_{i}", btn, partial(self._on_texture_item_clicked, i))
        self.add_element("texture_container", container)

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

        for element in self.element_types[pygame_gui.elements.UIScrollingContainer]:
            element_size = element.get_container().get_size()
            element.set_scrollable_area_dimensions(element_size)

    def handle_button(self, event):
        callback = self.element_callbacks[pygame_gui.elements.UIButton].get(
            event.ui_element, None
        )
        if callback:
            callback()

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
        if not self._area:
            raise ValueError("Area not set")
        return self._area

    @area.setter
    def surface(self, value: ScreenArea):
        self._area = value

    @property
    def root_element(self):
        return self.elements["root"]
