import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image, ImageTk

class ImageBoxTool:
    def __init__(self, root):
        self.root = root
        self.root.title("Image Box Tool")
        self.canvas = tk.Canvas(root, cursor="cross")
        self.canvas.pack(fill=tk.BOTH, expand=True)

        self.image = None
        self.image_tk = None
        self.image_path = None
        self.selection_boxes = []
        self.current_digit_index = 0
        self.digits = ["Units", "Tens", "Hundreds", "Thousands"]
        self.box_coordinates_list = []

        self.canvas.bind("<Button-1>", self.start_selection)
        self.canvas.bind("<B1-Motion>", self.update_selection)
        self.canvas.bind("<ButtonRelease-1>", self.end_selection)

        menu = tk.Menu(root)
        root.config(menu=menu)
        file_menu = tk.Menu(menu, tearoff=False)
        menu.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="Open Image", command=self.open_image)

    def open_image(self):
        file_path = filedialog.askopenfilename(filetypes=[("Image Files", "*.png;*.jpg;*.jpeg")])
        if file_path:
            self.image_path = file_path
            image = Image.open(file_path)
            self.image = ImageTk.PhotoImage(image)
            self.canvas.config(width=image.width, height=image.height)
            self.canvas.create_image(0, 0, anchor="nw", image=self.image)
            self.prompt_user()

    def prompt_user(self):
        if self.current_digit_index < len(self.digits):
            digit = self.digits[self.current_digit_index]
            messagebox.showinfo("Digit Selection", f"Select the {digit} digit by drawing a box around it.")
        else:
            self.display_summary()
            messagebox.showinfo("Digit Selection", "All digits have been selected. Proceed.")

    def start_selection(self, event):
        x, y = event.x, event.y
        self.box_coordinates = [x, y, x, y]
        self.selection_boxes.append(self.canvas.create_rectangle(x, y, x, y, outline="red"))

    def update_selection(self, event):
        x, y = event.x, event.y
        self.box_coordinates[2], self.box_coordinates[3] = x, y
        self.canvas.coords(self.selection_boxes[-1], self.box_coordinates[0], self.box_coordinates[1], x, y)

    def end_selection(self, event):
        units = self.box_coordinates
        self.box_coordinates_list.append(units)
        messagebox.showinfo("Box Selection", f"Box coordinates for {self.digits[self.current_digit_index]}: {units}")
        self.current_digit_index += 1
        self.selection_boxes.clear()
        self.prompt_user()

    def display_summary(self):
        print("Summary of Box Coordinates:")
        for i, digit_coords in enumerate(self.box_coordinates_list):
            print(f"{self.digits[i]}: {digit_coords}")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageBoxTool(root)
    root.mainloop()
