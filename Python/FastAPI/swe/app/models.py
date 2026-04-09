from dataclasses import dataclass, field
from datetime import datetime
from uuid import UUID, uuid4
from pydantic import BaseModel


# --- Data Models ---

@dataclass
class User:
    id: UUID = field(default_factory=uuid4)
    username: str = ""
    hashed_password: str = ""


@dataclass
class Topic(BaseModel):
    id: UUID = field(default_factory=uuid4)
    owner_id: UUID
    content: str
    hash: int
    ttl: datetime
    current_comment_number: int
    previous_comment_number: int
    total_comment_number: int


@dataclass
class Comment(BaseModel):
    id: UUID
    owner_id: UUID
    topic_id: UUID
    content: str


# --- In-Memory Store ---

db: dict = {
    "users":    {},  # UUID -> User
    "topics":   {},  # UUID -> Topic
    "comments": {},  # UUID -> Comment
}
