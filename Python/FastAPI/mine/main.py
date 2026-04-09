from datetime import datetime
from fastapi import FastAPI
from pydantic import BaseModel
from uuid import UUID

app = FastAPI()

class User(BaseModel):
    id: UUID
    username: str
    password: str

class Topic(BaseModel):
    id: UUID
    owner_id: UUID
    content: str
    ttl: datetime
    ccn: int
    tcn: int

class Comment(BaseModel):
    id: UUID
    user_id: UUID
    topic_id: UUID
    content: str


