import sqlite3

connection = sqlite3.connect("Control_Puerta.db")
crsr = connection.cursor()


sql_create_table_initial = """CREATE TABLE IF NOT EXISTS Personas_con_acceso ( 
identificador INTEGER PRIMARY KEY, 
fname VARCHAR(20), 
surname VARCHAR(30), 
gender CHAR(1), 
joining DATE);""" 

sql_create_table_entradas = """CREATE TABLE IF NOT EXISTS entradas (
    identificador INTEGER PRIMARY KEY,
    foto BLOB,
    fecha TEXT, --Formato: 'YYYY-MM-DD HH:MM:SS'
    aceptada_entrada TEXT --formato: 'YES' / 'NO'
)"""


crsr.execute(sql_create_table_initial)
crsr.execute(sql_create_table_entradas)
#primary_key
identificador = [1,2,3,4,5,6]
name = ["Mario", "Oscar","Lenin","Jose","Manuel","Laura"]
apellidos =  ["Bros", "Rodriguez", "Ivanov","Calasanz","Largo","Galindo"]
gender = ['M', 'M', 'M', 'M', 'M', 'F']
joining_date = [
    "2022-01-10",
    "2023-03-15",
    "2021-07-08",
    "2022-11-23",
    "2024-05-01",
    "2023-12-30"
]
for i in range(len(identificador)):
     crsr.execute(
        "INSERT INTO Personas_con_acceso VALUES (?, ?, ?, ?, ?)",
        (
            identificador[i],
            name[i],
            apellidos[i],
            gender[i],
            joining_date[i]
        )
    )
connection.commit()      
connection.close()