from pydantic import BaseModel, Field
from datetime import datetime
from uuid import UUID


# --- User ---

class UserRegister(BaseModel):
    username: str = Field(..., max_length=127)
    password: str


class UserLogin(BaseModel):
    username: str
    password: str


class UserOut(BaseModel):
    id: UUID
    username: str


# --- Token ---

class Token(BaseModel):
    access_token: str
    token_type: str


# --- Topic ---

class TopicCreate(BaseModel):
    content: str = Field(..., max_length=127)


class TopicOut(BaseModel):
    id: UUID
    owner_id: UUID
    content: str
    ttl: datetime
    total_comment_number: int


# --- Comment ---

class CommentCreate(BaseModel):
    content: str = Field(..., max_length=1023)


class CommentOut(BaseModel):
    id: UUID
    owner_id: UUID
    topic_id: UUID
    content: str
