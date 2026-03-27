# TYPES
def get_name_with_age(name: str, age: int):
    name_with_age = name.title() + " is " + str(age)
    return name_with_age

print(get_name_with_age("John Doe", 15))

# UNION
def say_hi(name: str | None = None):
    if name is not None:
        print(f"Hey {name}")
    else:
        print("Hello World")

say_hi()
say_hi("Some Name")

# CLASS
class Person:
    def __init__(self, name: str):
        self.name = name

def get_person_name(one_person: Person):
    return one_person.name

person = Person("Person1")
print(get_person_name(person))
print(get_person_name(Person("Person2")))

# PYDANTIC
from datetime import datetime
from pydantic import BaseModel

class User(BaseModel):
    id: int
    name: str = "John Doe Pydantic"
    signup_ts: datetime | None = None
    friends: list[int] = []

external_data = {
    "id": "123",
    "signup_ts": "2017-06-01 12:22",
    "friends": [1, "2", b"3"],
}

user = User(**external_data)
print(user)
print(f"Id: {user.id}")

# ANNOTATED
from typing import Annotated

def say_hello(name: Annotated[str, "this is just metadata"]) -> str:
    return f"Hello {name}"

print(say_hello("Annotated"))
