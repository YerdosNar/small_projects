from dataclasses import dataclass, field
from datetime import datetime
from uuid import UUID, uuid4


# --- Data Models ---

@dataclass
class User:
    id: UUID = field(default_factory=uuid4)
    username: str = ""
    hashed_password: str = ""


@dataclass
class Topic:
    id: UUID = field(default_factory=uuid4)
    owner_id: UUID = None
    content: str = ""
    hash: int = 0
    ttl: datetime = None
    current_comment_number: int = 0
    previous_comment_number: int = 0
    total_comment_number: int = 0


@dataclass
class Comment:
    id: UUID = field(default_factory=uuid4)
    owner_id: UUID = None
    topic_id: UUID = None
    content: str = ""


# --- In-Memory Store ---

db: dict = {
    "users":    {},  # UUID -> User
    "topics":   {},  # UUID -> Topic
    "comments": {},  # UUID -> Comment
}
