from fastapi import APIRouter, HTTPException, status
from app.models import db, User
from app.schemas import UserRegister, UserOut, UserLogin, Token
from app.dependencies import hash_password, verify_password, create_access_token

router = APIRouter(prefix="/auth", tags=["Auth"])


# --- Register ---

@router.post("/register", response_model=UserOut, status_code=status.HTTP_201_CREATED)
def register(payload: UserRegister):
    # Check username availability
    for user in db["users"].values():
        if user.username == payload.username:
            raise HTTPException(
                status_code=status.HTTP_409_CONFLICT,
                detail="Username already taken"
            )

    new_user = User(
        username=payload.username,
        hashed_password=hash_password(payload.password)
    )
    db["users"][new_user.id] = new_user
    return new_user


# --- Login ---

@router.post("/login", response_model=Token)
def login(payload: UserLogin):
    # Find user by username
    user = next(
        (u for u in db["users"].values() if u.username == payload.username),
        None
    )
    if not user or not verify_password(payload.password, user.hashed_password):
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid username or password"
        )

    token = create_access_token(data={"sub": str(user.id)})
    return {"access_token": token, "token_type": "bearer"}
