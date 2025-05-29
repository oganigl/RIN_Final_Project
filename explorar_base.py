import sqlite3
import io
from tkinter import Tk, ttk, Label, Scrollbar, Canvas, Frame
from PIL import Image, ImageTk

class DBViewer:
    def __init__(self, db_path="Control_Puerta.db"):
        self.conn = sqlite3.connect(db_path)
        self.crsr = self.conn.cursor()
        
        self.root = Tk()
        self.root.title("Visor Base de Datos con Imágenes")
        self.frame = Frame(self.root)
        self.frame.pack(fill='both', expand=True)
        
        # Scroll vertical para canvas
        self.canvas = Canvas(self.frame)
        self.scrollbar = Scrollbar(self.frame, orient="vertical", command=self.canvas.yview)
        self.scrollable_frame = Frame(self.canvas)
        
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(
                scrollregion=self.canvas.bbox("all")
            )
        )
        
        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        
        self.canvas.pack(side="left", fill="both", expand=True)
        self.scrollbar.pack(side="right", fill="y")
        
        self.imagenes = []  # Guardar referencias a imágenes para que tkinter no las borre
        
        self.mostrar_datos()
        
        self.root.mainloop()
        
    def mostrar_datos(self):
        query = """
            SELECT 
                entradas.identificador,
                Personas_con_acceso.fname,
                Personas_con_acceso.surname,
                entradas.fecha,
                entradas.aceptada_entrada,
                entradas.foto
            FROM entradas
            LEFT JOIN Personas_con_acceso ON entradas.identificador = Personas_con_acceso.identificador
            ORDER BY entradas.fecha;
        """

        self.crsr.execute(query)
        filas = self.crsr.fetchall()
        
        # Header
        headers = ["ID","NAME","SURNAME","Fecha","Aceptada","Foto"]
        for i, h in enumerate(headers):
            lbl = Label(self.scrollable_frame, text=h, font=('Arial', 12, 'bold'), borderwidth=1, relief="solid", width=20)
            lbl.grid(row=0, column=i, sticky='nsew')
        
        for idx, fila in enumerate(filas, start=1):
            id,name,surname,fecha, aceptada,foto_blob  = fila
            
            Label(self.scrollable_frame, text=str(id), borderwidth=1, relief="solid", width=20).grid(row=idx, column=0)
            Label(self.scrollable_frame, text=str(name), borderwidth=1, relief="solid", width=20).grid(row=idx, column=1)
            Label(self.scrollable_frame, text=str(surname), borderwidth=1, relief="solid", width=20).grid(row=idx, column=2)
            Label(self.scrollable_frame, text=str(fecha), borderwidth=1, relief="solid", width=20).grid(row=idx, column=3)
            Label(self.scrollable_frame, text=aceptada, borderwidth=1, relief="solid", width=20).grid(row=idx, column=4)
            # Convertir BLOB a imagen y mostrar thumbnail
            if foto_blob:
                image = Image.open(io.BytesIO(foto_blob))
                image.thumbnail((100, 100))
                photo = ImageTk.PhotoImage(image)
                self.imagenes.append(photo)  # Evitar que la imagen se borre
                lbl_img = Label(self.scrollable_frame, image=photo, borderwidth=1, relief="solid")
                lbl_img.grid(row=idx, column=5)
            else:
                Label(self.scrollable_frame, text="Sin Imagen", borderwidth=1, relief="solid", width=20).grid(row=idx, column=2)
            
            

if __name__ == "__main__":
    DBViewer()
